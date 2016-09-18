template <typename _T> template <typename _U> uint32_t  Exchange<_T>::order_id<_U>::value = 0;

template <typename _T>
Exchange<_T>::Exchange(feed_ptr_t feed, real_t cash) : base_t(feed, cash) 
{
    log_debug("Exchange<{}>::{}", type_name<_T>(), __func__);
}

template <typename _T>
Exchange<_T>::~Exchange()
{
    log_debug("Exchange<{}>::{}", type_name<_T>(), __func__);
}

template <typename _T>
bool Exchange<_T>::submit_order(order_ptr_t order) {
//    log_trace("Exchange::{} order id={} current date={} (entry)", __func__,  order->id, to_str(base_t::feed->current_date));

    log_trace("Exchange<_T>::{} (entry)", __func__);
    bool ret = false;
    if (order->is_initial()) 
    {
        assert(this->feed);
        const datetime_t& current_datetime = this->feed->current_date;
        //assert (current_datetime != nulldate);
        log_debug("Exchange<_T>::{} current date={}", __func__, to_str(current_datetime));
        order->set_submitted(++order_id_t::value, current_datetime);
        ret = this->register_order(order);
        if (ret) 
        {
            // @deprecatecomment Switch from INITIAL -> SUBMITTED
            // IMPORTANT: Do not emit an event for this switch because when using the position interface
            // the order is not yet mapped to the position and Position.onOrderUpdated will get called.
            order->switch_state(order_t::Submitted);
            base_t::event_ptr_t event(new order_t::Event(current_datetime, order.get(), order_t::Event::Submitted, base_t::info_ptr_t(nullptr)));
            this->order_event.emit(*this, event);
        }
        else
        {
            log_error("Exchange<_T>::{} failed to register order id={}", __func__, order->id);
//            order->switch_state(order_t::Canceled);
//            base_t::event_ptr_t event(new order_t::Event(current_datetime, order.get(), order_t::Event::Canceled, base_t::info_ptr_t(nullptr)));
//            this->order_event.emit(*this, event);
        }
    } 
    else 
    {
        log_error("Exchange<_T>::{} order id={} invalid state={}", __func__, order->id, order_t::to_str(order->state));
    }
    
    log_trace("Exchange<_T>::{} (exit)", __func__);
    
    return ret;
}

template <typename _T>
bool Exchange<_T>::cancel_order(uint32_t id) 
{
//    log_trace("Exchang<_T>::{} order id={}", __func__, id);
    bool ret = false;
    try {
        order_t* order = this->orders_.at(id).get();
        if (!order->is_active()) {
            log_error("Exchange<_T>::{} order id={} is not active", __func__, id);
            return ret;
        }

        if (order->is_filled()) {
            log_error("Exchange<_T>::{} can't cancel, order id={} already filled",__func__, id);
            return ret;
        }

        this->unregister_order(id);
        order->switch_state(order_t::Canceled);
        base_t::event_ptr_t event(new order_t::Event(this->feed->current_date, order, order_t::Event::Canceled, base_t::info_ptr_t(nullptr)));
        this->order_event.emit(*this, event);
        ret = true;
    } 
    catch (...) 
    {
        log_error("Exchange<_T>::{} order id={} not found from active orders", __func__, id);
    }
    return ret;
}

template <typename _T>
real_t Exchange<_T>::get_equity() 
{
    real_t ret = this->cash;         
    const values_t& values = this->feed->current_values;
    for (const base_t::shares_t::value_type& share : this->shares) 
    {
        values_t::const_iterator iter = values.find(share.first);
        if (iter != values.end()) 
        {
            const value_t& value = iter->second;
            ret += value.close * share.second;
        }
    }
    return ret;
}

template <typename _T>
typename Exchange<_T>::order_ptr_t Exchange<_T>::create_order(order_t::Type type, order_t::Action action, const std::string& symbol, size_t quantity, 
        bool isFillOnClose, real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone) 
{
    order_ptr_t ptr(new order_t(type, action, symbol, quantity, isFillOnClose, stopPrice, limitPrice));
    if (ptr)
    {
        ptr->is_good_till_canceled = isGoodTillCanceled;
        ptr->is_all_or_none = isAllOrNone;
        this->submit_order(ptr);
    }
    return ptr;
}
