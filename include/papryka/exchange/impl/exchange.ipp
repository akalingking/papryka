template <typename _U> uint32_t  Exchange::order_id<_U>::value = 0;

Exchange::Exchange(feed_ptr_t feed, real_t cash) : base_t(feed, cash) 
{
    log_debug("Exchange created");
}

bool Exchange::submit_order(order_ptr_t order) {
//    log_trace("ExchangeBase::{} order id={} current date={}", __func__, 
//            order->id, to_str(base_t::feed->current_date));

    bool ret = false;
    if (order->is_initial()) 
    {
        order->set_submitted(++order_id_t::value, feed->current_date);
        base_t::register_order(order);
        // Switch from INITIAL -> SUBMITTED
        // IMPORTANT: Do not emit an event for this switch because when using the position interface
        // the order is not yet mapped to the position and Position.onOrderUpdated will get called.
        order->switch_state(order_t::Submitted);
        //order_event.emit(...)
        ret = true;
    } else {
        log_error("ExchangeBase::{} order id={} invalid state={}", __func__, order->id, order_t::to_str(order->state));
    }
    return ret;
}

bool Exchange::cancel_order(uint32_t id) {
//    log_trace("ExchangeBase::{} order id={}", __func__, id);
    bool ret = false;
    try {
        order_t* order = orders_.at(id).get();
        if (!order->is_active()) {
            log_error("exchange_base::{} order id={} is not active", __func__, id);
            return ret;
        }

        if (order->is_filled()) {
            log_error("ExchangeBase::{} can't cancel, order id={} already filled",__func__, id);
            return ret;
        }

        unregister_order(id);
        order->switch_state(order_t::Canceled);
        event_ptr_t event(new order_t::Event(feed->current_date, order, order_t::Event::Canceled, info_ptr_t(nullptr)));
        order_event.emit(*this, event);
        ret = true;
    } catch (...) {
        log_error("ExchangeBase::{} order id={} not found from active orders", __func__, id);
    }
    return ret;
}

real_t Exchange::get_equity() {
    real_t ret = cash;         
    const values_t& values = feed->current_values;
    for (const shares_t::value_type& share : shares) {
        values_t::const_iterator iter = values.find(share.first);
        if (iter != values.end()) {
            const value_t& value = iter->second;
            ret += value.close * share.second;
        }
    }
    return ret;
}

Exchange::order_ptr_t Exchange::create_order(order_t::Type type, order_t::Action action, const std::string& symbol, size_t quantity, bool isFillOnClose, real_t limitPrice, real_t stopPrice) 
{
    return order_ptr_t(new order_t(type, action, symbol, quantity, isFillOnClose, limitPrice, stopPrice));
}