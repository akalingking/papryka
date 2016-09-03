/**
 * Returns the trigger price for a Stop or StopLimit order, 
 * or 0.0 if the stop price was not yet penetrated.
 * @param action
 * @param stopPrice
 * @param useAdjValues
 * @param bar
 * @return 
 */
real_t FillTrigger::get_stop_price_trigger(Order::Action action, real_t stopPrice, bool useAdjValues, const value_t& bar)
{
    real_t ret = real_t(0.0);
    real_t open = bar.open;
    real_t high = bar.high;
    real_t low = bar.low;
    // If the bar is above the stop price, use the open price.
    // If the bar includes the stop price, use the open price 
    // or the stop price. Whichever is better.
    if (action == Order::Action::Buy || action == Order::Action::BuyToCover)
    {
        if (low > stopPrice)
            ret = open;
        else if (stopPrice <= high)
        {
            if (open > stopPrice) // The stop price was penetrated on open.
                ret = open;
            else
                ret = stopPrice;
        }
    }
        // If the bar is below the stop price, use the open price.
        // If the bar includes the stop price, use the open price 
        // or the stop price. Whichever is better.
    else if (action == Order::Action::Sell || action == Order::Action::SellShort)
    {
        if (high < stopPrice)
            ret = open;
        else if (stopPrice >= low)
        {
            if (open < stopPrice) // The stop price was penetrated on open.
                ret = open;
            else
                ret = stopPrice;
        }
    }
    else
        assert(false);

    return ret;
}

// Returns the trigger price for a Limit or StopLimit order, 
// or 0.0 if the limit price was not yet penetrated.

real_t FillTrigger::get_limit_price_trigger(Order::Action action, real_t limitPrice, bool useAdjValues, const value_t& bar)
{
    real_t ret = real_t(0.0);
    real_t open = bar.open;
    real_t high = bar.high;
    real_t low = bar.low;

    // If the bar is below the limit price, use the open price.
    // If the bar includes the limit price, use the open price or the limit price.
    if (action == Order::Action::Buy || action == Order::Action::BuyToCover)
    {
        if (high < limitPrice)
        {
            ret = open;
        }
        else if (limitPrice >= low)
        {
            if (open < limitPrice) // The limit price was penetrated on open.
                ret = open;
            else
                ret = limitPrice;
        }
    }
        // If the bar is above the limit price, use the open price.
        // If the bar includes the limit price, use the open price or the limit price.
    else if (action == Order::Action::Sell || action == Order::Action::SellShort)
    {
        if (low > limitPrice)
        {
            ret = open;
        }
        else if (limitPrice <= high)
        {
            if (open > limitPrice) // The limit price was penetrated on open.
                ret = open;
            else
                ret = limitPrice;
        }
    }
    else
        assert(false);

    return ret;
}

FillType<Bar>::FillType(Frequency frequency, real_t volumeLimit) :
        frequency(frequency), volume_limit_(volumeLimit) 
{}

void FillType<Bar>::on_bars(const datetime_t& date, const values_t& values)
{
    log_trace("FillType<Bar>::{} {} (entry)", __func__, to_str(date));
    
    typename values_t::const_iterator iter;
    for (iter = values.begin(); iter != values.end(); ++iter)
    {
        const std::string& symbol = iter->first;
        const value_t& bar = iter->second;
        real_t volume_left = real_t(0.0);
        if (frequency == Frequency::Tick)
            volume_left = bar.volume;
        else if (volume_limit_ != real_t(0.0))
            volume_left = bar.volume * volume_limit_;
        
        volume_left_.insert(volumes_t::value_type(symbol, volume_left));
        volume_used_.insert(volumes_t::value_type(symbol, real_t(0.0)));
    }
    log_trace("FillType<Bar>::{} (exit)", __func__, to_str(date));
}

real_t FillType<Bar>::get_volume_left(const std::string& symbol)
{
    real_t ret = real_t(0.0);
    volumes_t::const_iterator iter = volume_left_.find(symbol);
    if (iter != volume_left_.end())
         ret = iter->second;
    return ret;
}

template <typename _Order>
void FillType<Bar>::on_order_filled(_Order& order)
{
    log_trace("FillType::{} order id={} order qty={} filled={}", __func__, order.id, order.quantity, order.filled);

    const std::string& symbol = order.symbol;
    const Order::Info* info = order.info.get();

    size_t order_size = info->quantity;
    real_t volume_left = volume_left_[symbol];
    real_t volume_used = volume_used_[symbol];

    if (volume_limit_ != real_t(0.0))
    {
        if (order_size > volume_left)
            log_error("FillType::{0:} {1:} order qty={2:} volume left={3:0.3f}", __func__, symbol, order_size, volume_left);
        else
            volume_left_[symbol] = precision::round(volume_left - order_size);
    }

    volume_used_[symbol] = precision::round(volume_used + order_size);
}

template <typename _Order>
FillType<Bar>::info_ptr_t FillType<Bar>::fill(_Order& order, const value_t& value, market_order_tag&)
{
    info_ptr_t ret = nullptr;
    real_t fill_size = calculate_fill_size_(order, value);

    if (fill_size == 0)
    {
        log_error("FillType::{0:} not enough volume to fill {1:} market order [id={2:}] for {3:0.3f} share/s.", __func__, order.symbol, order.id, order.get_remaining());
    }
    else
    {
        real_t price = real_t(0.0);
        if (order.is_fill_on_close)
            price = value.close;
        else
            price = value.open;

        if (price > real_t(0.0))
        {
            if (frequency != Frequency::Tick)
            {
                const std::string& symbol = order.symbol;
                price = slippage_.calculate_price(order, value, price, fill_size, volume_used_[symbol]);
            }
            ret.reset(new FillInfo(price, fill_size));
        }
    }
    return ret;
}

template <typename _Order>
FillType<Bar>::info_ptr_t FillType<Bar>::fill(_Order& order, const value_t& bar, limit_order_tag&)
{
    info_ptr_t ret = nullptr;
    real_t fill_size = real_t(0); //calculate_fill_size_(order, bar);

    if (fill_size == real_t(0))
    {
        log_error("FillType::{} not enough volume to fill {} stop order id={} for {0.3f} share/s", __func__,
            order.symbol, order.id, order.get_remaining());
    }
    else
    {
        real_t price = FillTrigger().get_limit_price_trigger(order.action, order.limit_price, false, bar);
        order.is_limit_hit = price != real_t(0.0);
        if (price != real_t(0.0))
            ret.reset(new FillInfo(price, fill_size));
    }

    return ret;
}

template <typename _Order>
FillType<Bar>::info_ptr_t FillType<Bar>::fill_stop_order(_Order& order, const value_t& bar, stop_order_tag&)
{
    info_ptr_t ret = nullptr;
    real_t stop_price_trigger = real_t(0);
    if (!order.is_stop_hit)
    {
        stop_price_trigger = FillTrigger().get_stop_price_trigger(order.action, order.stop_price, false, bar);
        order.is_stop_hit = (stop_price_trigger != real_t(0));
    }

    if (order.is_stop_hit)
    {
        int fill_size = calculate_fill_size_(order, bar);
        if (fill_size == 0)
        {
            log_debug("FillType::{} not enough volume to fill "
                "{} stop order id={} for {} share/s", __func__, order.symbol, order.id, order.get_remaining());
        }
        else
        {
            real_t price = real_t(0.0);
            if (stop_price_trigger != 0)
                price = stop_price_trigger;
            else
                price = bar.open;
            ret.reset(new FillInfo(price, fill_size));
        }
    }
    return ret;
}

template <typename _Order>
FillType<Bar>::info_ptr_t FillType<Bar>::fill(_Order& order, const value_t& bar, stop_limit_order_tag&)
{
    info_ptr_t ret = nullptr;
    real_t stop_price_trigger = real_t(0);
    if (!order.is_stop_hit)
    {
        stop_price_trigger = FillTrigger().get_stop_price_trigger(order.action, order.stop_price, false, bar);
        order.is_stop_hit = (stop_price_trigger != 0);
    }

    if (order.is_stop_hit)
    {
        int fill_size = calculate_fill_size_(order, bar);

        if (fill_size == 0)
        {
            log_error("FillType::{} not enough volume to fill "
                "{} stop limit order id={} for {} share/s", __func__,
                order.symbol, order.id, order.get_remaining());
        }
        else
        {
            real_t price = FillTrigger().get_limit_price_trigger(order.action, order.limit_price, false, bar);
            order.is_limit_hit = price != real_t(0.0);

            if (price != 0)
            {
                if (stop_price_trigger != 0)
                {
                    if (order.is_buy())
                        price = std::min(stop_price_trigger, order.limit_price);
                    else
                        price = std::max(stop_price_trigger, order.limit_price);
                }
                ret.reset(new FillInfo(price, fill_size));
            }
        }
    }
    return ret;
}

template <typename _Order>
real_t FillType<Bar>::calculate_fill_size_(_Order& order, const value_t& bar)
{
    real_t ret = real_t(0.0), volume_left = real_t(0.0);
    real_t order_qty_remaining = order.get_remaining();
    volumes_t::iterator iter = volume_left_.find(order.symbol);
    if (iter != volume_left_.end())
    {
        if (volume_limit_ != 0)
            volume_left = precision::round(iter->second);
        else
            volume_left = order_qty_remaining;

        log_debug("FillType::{0:} {1:} volume left={2:0.3f} order remaining={3:0.3f}", __func__, order.symbol, volume_left, order_qty_remaining);

        if (!order.is_all_or_none)
            ret = std::min(volume_left, order_qty_remaining);
        else if (order_qty_remaining <= volume_left)
            ret = order_qty_remaining;
        else
            assert(false);
    }
    else
    {
        log_error("FillType::{0:} {1:} has no volume left data!", __func__, order.symbol);
        assert(false);
    }

    return ret;
}