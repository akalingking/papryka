
template <typename _T, typename _Fill, typename _Commission>
Exchange<_T, _Fill, _Commission>::Exchange(feed_ptr_t feed, real_t cash) :
    feed(feed), cash(cash), fillstrategy(feed->frequency)
{
    feed->new_values_event.subscribe(&Exchange<_T, _Fill, _Commission>::on_bars, this);
}

template <typename _T, typename _Fill, typename _Commission>
bool Exchange<_T, _Fill, _Commission>::register_order(order_ptr_t order)
{
    return orders_.insert(typename orders_t::value_type(order->id, order)).second;
}

template <typename _T, typename _Fill, typename _Commission>
void Exchange<_T, _Fill, _Commission>::unregister_order(uint32_t id)
{
    orders_.erase(id);
}

template <typename _T, typename _Fill, typename _Commission>
void Exchange<_T, _Fill, _Commission>::on_bars(const datetime_t& datetime, const values_t& values)
{
    log_trace("Exchange:{} {}", __func__, to_str(datetime));
    fillstrategy.on_bars(datetime, values);
    typename orders_t::iterator iter = orders_.begin();
    for (; iter != orders_.end(); ++iter)
        on_bars_imp(datetime, values, iter->second.get());
}

template <typename _T, typename _Fill, typename _Commission>
void Exchange<_T, _Fill, _Commission>::on_bars_imp(const datetime_t& datetime, const values_t& values, order_t* order)
{
    log_debug("Exchange:{} {} order id={}", __func__, to_str(datetime), order->id);
    const std::string& symbol = order->symbol;
    typename values_t::const_iterator iter = values.find(symbol);
    if (iter == values.end())
    {
        log_error("Exchange:{} symbol={} not found!", __func__, order->symbol);
        assert(false);
        return;
    }

    const value_t& value = iter->second;
    // Check if there is a valid bar for the order instrument
    if (datetime != nulldate)
    {
        if (order->is_submitted())
        {
            order->accepted_date = datetime;
            order->switch_state(order_t::Accepted);
            event_ptr_t event(new typename order_t::Event(datetime, order, order_t::Event::Accepted, info_ptr_t(nullptr)));
            order_event.emit(*this, event);
        }

        if (order->is_active())
        {
            process_order(datetime, value, order);
        }
        else
        {
            assert(order->is_cancelled());
            assert(orders_.find(order->id) == orders_.end());
        }
    }
}

template <typename _T, typename _Fill, typename _Commission>
void Exchange<_T, _Fill, _Commission>::process_order(const datetime_t& datetime, const value_t& value, order_t* order)
{
    log_debug("Exchange::{} order id={} qty={} current date={}", __func__, order->id, order->quantity, to_str(datetime));

    if (!pre_process_order(datetime, value, order))
        return;

    typename _Fill::info_ptr_t fill_info = order->process(fillstrategy, value);

    if (fill_info.get() != nullptr)
    {
        assert(fill_info->price != 0);
        assert(fill_info->quantity != 0);
        commit_order_execution(datetime, order, fill_info.get());
    }

    if (order->is_active())
        post_process_order(datetime, value, order);
}

template <typename _T, typename _Fill, typename _Commission>
bool Exchange<_T, _Fill, _Commission>::pre_process_order(const datetime_t& datetime, const value_t& value, order_t* order)
{
    log_trace("Exchange::{} order id={} state={} current date={} accepted date={}", 
            __func__, order->id, order_t::to_str(order->state), to_str(datetime), to_str(order->accepted_date));

    bool ret = true;
    if (!order->is_good_till_canceled)
    {
        // NOTE: Order expires if time delta is more than a day.
        date_t bar_date = to_date(datetime);
        date_t accepted_date = to_date(order->accepted_date);
        log_debug("Exchange::{} bar date={}", __func__, to_str(bar_date));
        log_debug("Exchange::{} accepted date {}", __func__, to_str(accepted_date));
        bool expired = to_date(datetime) > to_date(order->accepted_date);
        if (expired)
        {
            log_warn("Exchange::{} order id={} has expired, bar date={} accepted date={}", __func__, order->id, to_str(datetime), to_str(order->accepted_date));
            
            order->switch_state(order_t::Canceled);
            event_ptr_t event(new typename order_t::Event(datetime, order, order_t::Event::Canceled, info_ptr_t(nullptr)));
            order_event.emit(*this, event);
            
            unregister_order(order->id);
            
            ret = false;
        }
    }

    return ret;
}

template <typename _T, typename _Fill, typename _Commission>
void Exchange<_T, _Fill, _Commission>::post_process_order(const datetime_t& datetime, const value_t& value, order_t* order)
{
    log_debug("Exchange::{} order id={} current date={} accepted date={}", 
            __func__, order->id, to_str(datetime), to_str(order->accepted_date));

    if (!order->is_good_till_canceled)
    {
        //only accept orders within the day.
        bool expired = to_date(datetime) > to_date(order->accepted_date);
        if (expired)
        {
            log_error("Exchange::{} order id={} has expired", __func__, order->id);
            
            // emit event first before destroying order instance in unregister_order
            order->switch_state(order_t::Canceled);
            event_ptr_t event(new typename order_t::Event(datetime, order, order_t::Event::Canceled, info_ptr_t(nullptr)));
            order_event.emit(*this, event);
            
            unregister_order(order->id);
        }
    }
}

template <typename _T, typename _Fill, typename _Commission>
bool Exchange<_T, _Fill, _Commission>::commit_order_execution(const datetime_t& datetime, order_t* order, fill_info_t* fill)
{
    log_debug("Exchange::{} date={} id={} order qty={}, filled={}, new fill={}", __func__, papryka::to_str(datetime), order->id, order->quantity, order->filled, fill->quantity);

    bool ret = false;
    real_t price = fill->price;
    size_t quantity = fill->quantity;
    real_t cost = real_t(0.0);
    size_t shares_delta = size_t(0.0);
    const std::string& symbol = order->symbol;

    if (order->is_buy())
    {
        cost = price * quantity * real_t(-1.0);
        shares_delta = quantity;
    }
    else if (order->is_sell())
    {
        cost = price * quantity;
        shares_delta = quantity * real_t(-1.0);
    }

    real_t commission = this->commission.calculate(*order, price, quantity);
    cost -= commission;
    real_t resulting_cash = this->cash + cost;

    // Check cash after commission
    if (resulting_cash >= 0 || allow_negative_cash)
    {
        // commit transaction
        info_ptr_t info(new typename order_t::Info(price, quantity, commission, datetime));
        order->add_info(info);

        this->cash = resulting_cash;

        size_t updated_shares = precision_.round(shares[symbol] + shares_delta);

        if (updated_shares == 0)
            shares.erase(symbol);
        else
            shares[symbol] = updated_shares;

        fillstrategy.on_order_filled(*order);

        event_ptr_t event;
        if (order->is_filled())
        {
            event.reset(new typename order_t::Event(datetime, order, order_t::Event::Filled, info));
            order_event.emit(*this, event);
            orders_.erase(order->id);
        }
        else if (order->is_partially_filled())
        {
            event.reset(new typename order_t::Event(datetime, order, order_t::Event::PartiallyFilled, info));
            order_event.emit(*this, event);
        }

        ret = true;
    }
    else
    {
        log_error("Exchange::{} order id={} not enough cash to fill {} share(s) shares, cash={0.3f}, needed={0.3f}",
            __func__, order->id, quantity, cash, resulting_cash * real_t(-1.0));
    }
    return ret;
}
