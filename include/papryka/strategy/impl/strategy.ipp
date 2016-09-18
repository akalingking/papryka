
/**
 * Copyright 2015 Ariel Kalingking <akalingking@gmail.com>
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
template <typename _D, typename _T>
Strategy<_D,_T>::Strategy(exchange_ptr_t exchange) : feed(exchange->feed), exchange(exchange)
{
//    symbols_ = feed->get_symbols();
    
    exchange->order_event.subscribe(&Strategy<_D,_T>::on_order_event_, this);
    
    // Strategy must subscribe to bar events after the exchange.
    feed->new_values_event.subscribe(&Strategy<_D,_T>::on_bars_, this);
    
    dispatcher_.start_event.subscribe(&Strategy<_D,_T>::on_start_, this);
    
    dispatcher_.idle_event.subscribe(&Strategy<_D,_T>::on_idle_, this);
    
    dispatcher_.stop_event.subscribe(&Strategy<_D,_T>::on_stop_, this);
    
    // Exchange must dispatch events first, especially when backtesting.
    // exchange now should be driven by feed events
    // dispatcher_.add_subject(exchange_);
    
    dispatcher_.add_subject(feed);
    
    log_debug("Strategy<{}>::{}", type_name<_D>(), __func__);
}

template <typename _D, typename _T>
Strategy<_D,_T>::~Strategy()
{
    log_debug("Strategy<{}>::{} entry", type_name<_D>(), __func__);
    exchange.reset();
    log_debug("Strategy<{}>::{} exit", type_name<_D>(), __func__);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::run() 
{ 
    log_debug("Strategy::{} entry", __func__);
    dispatcher_.start();
    dispatcher_.wait();
    log_debug("Strategy::{} exit", __func__);
    
}

template <typename _D, typename _T>
void Strategy<_D,_T>::on_bars_(const datetime_t& date, const values_t& values)
{
#if defined(ENABLE_ANALYZERS)
    analyzers_.before_on_bars(*this, values);
#endif
    (static_cast<_D&>(*this)).on_bars(date, values);
    
    bars_processed_event.emit(*this, values);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::on_order_event_(exchange_t& exchange, order_event_ptr_t orderEvent)
{
    assert (orderEvent.get() != nullptr);
    order_t* order = (order_t*)orderEvent->order;
    assert (order != nullptr);
    const uint32_t& order_id = order->id;
   
    log_debug("Strategy::{} {} order id={} order type={} order event type={} order state={}", 
            __func__, to_str(orderEvent->datetime), order_id, order_t::to_str(order->type), order_t::Event::to_str(orderEvent->type), order_t::to_str(order->state));
    
    (static_cast<_D&>(*this)).on_order_updated(order);
    
    order_to_position_t::iterator order_to_pos_iter = order_to_position_.find(order_id);
    if (order_to_pos_iter != order_to_position_.end())
    {
        const uint32_t& pos_id = order_to_pos_iter->second;
        assert (pos_id != 0);
        typename positions_t::iterator pos_iter = active_positions_.find(pos_id);       
        if (pos_iter != active_positions_.end())
        {
            position_t* pos = pos_iter->second.get();            
            assert (pos != nullptr);
            if (pos != nullptr)
            {
                // emit event first, before possibly getting destroyed
                pos->on_order_event(orderEvent);
                
                if (!order->is_active())
                {
                    log_debug("Strategy::{} unlink order id={} from position id={}", __func__, order_id, pos_id);
                    unregister_position_order(pos_id, order_id);
                }
            }
        }
        else
        {
            log_warn("Strategy::{} position id={} is not on active list", __func__, pos_id);
        }
    }
    else
    {
        log_debug("Strategy::{} order id={} is not on active list", __func__, order_id);
    }
}

template <typename _D, typename _T>
void Strategy<_D,_T>::register_position(position_ptr_t pos)
{
    assert (pos);
    const uint32_t& posId = pos->id;
    log_trace("Strategy::{} position id={}", __func__, posId);
    if (active_positions_.find(posId) == active_positions_.end())
        active_positions_.insert(typename positions_t::value_type(posId, pos));
    else
        log_warn("Strategy::{} position id={} already registered", __func__, posId);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::unregister_position(uint32_t posId)
{
    log_trace("Strategy<_D,_T>::{} position id={}", __func__, posId);
    assert (active_positions_.find(posId) != active_positions_.end());
    active_positions_.erase(posId);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::register_position_order(uint32_t posId, uint32_t orderId)
{
    log_trace("Strategy::{} order id={} position id={}", __func__, orderId, posId);
    order_to_position_[orderId] = posId;
}

template <typename _D, typename _T>
void Strategy<_D,_T>::unregister_position_order(uint32_t posId, uint32_t orderId)
{
    assert(order_to_position_.find(orderId) != order_to_position_.end());
    order_to_position_.erase(orderId);
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::get_position(const uint32_t id)
{
    position_t* pos = nullptr;
    typename positions_t::iterator iter = active_positions_.find(id);
    if (iter != active_positions_.end())
        pos = iter->second.get();
    return pos;
}

template <typename _D, typename _T>
real_t Strategy<_D,_T>::get_result()
{
    return exchange->get_equity();
}

template <typename _D, typename _T>
real_t Strategy<_D,_T>::get_last_price(const std::string& symbol) const
{
    real_t ret = real_t(0.0);
    const value_t& value = feed->current_values[symbol];
    ret = value.close;
    return ret;
}

template <typename _D, typename _T>
const datetime_t& Strategy<_D,_T>::get_current_datetime() const 
{ 
    return feed->current_date;
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::order_ptr_t Strategy<_D,_T>::create_order(typename order_t::Type type, typename order_t::Action action, const std::string& symbol, size_t quantity, 
        bool isFillOnClose, real_t stopPrice, real_t limitPrice)
{
    return exchange->create_order(type, action, symbol, quantity, isFillOnClose, stopPrice, limitPrice);            
}
    
template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::enter_long(const std::string& symbol, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new LongPosition<_D,_T>(*this, symbol, real_t(0), real_t(0), quantity, isGoodTillCanceled, isAllOrNone);
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::enter_short(const std::string& symbol, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new ShortPosition<_D,_T>(*this, symbol, real_t(0), real_t(0), quantity, isGoodTillCanceled, isAllOrNone);
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::enter_long_stop(const std::string& symbol, size_t quantity, real_t stopPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new LongPosition<_D,_T>(*this, symbol, stopPrice, real_t(0), quantity, isGoodTillCanceled, isAllOrNone);
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::enter_short_stop(const std::string& symbol, size_t quantity, real_t stopPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new ShortPosition<_D,_T>(*this, symbol, stopPrice, real_t(0), quantity, isGoodTillCanceled, isAllOrNone);
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::enter_long_limit(const std::string& symbol, size_t quantity, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new LongPosition<_D,_T>(*this, symbol, real_t(0), limitPrice, quantity, isGoodTillCanceled, isAllOrNone);
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::enter_short_limit(const std::string& symbol, size_t quantity, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new ShortPosition<_D,_T>(*this, symbol, real_t(0), limitPrice, quantity, isGoodTillCanceled, isAllOrNone);
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::enter_long_stop_limit(const std::string& symbol, size_t quantity, 
        real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new LongPosition<_D,_T>(*this, symbol, stopPrice, limitPrice, quantity, isGoodTillCanceled, isAllOrNone);
}

template <typename _D, typename _T>
typename Strategy<_D,_T>::position_t* Strategy<_D,_T>::enter_short_stop_limit(const std::string& symbol, size_t quantity, 
        real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new ShortPosition<_D,_T>(*this, symbol, stopPrice, limitPrice, quantity, isGoodTillCanceled, isAllOrNone);
}

