
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
Strategy::Strategy(exchange_ptr_t& exchange) : 
        feed(exchange->feed), exchange(exchange)
{
//    symbols_ = feed->get_symbols();
    
    exchange->order_event.subscribe(&Strategy::on_order_event_, this);
    
    // Strategy must subscribe to bar events after the exchange.
    feed->new_values_event.subscribe(&Strategy::on_bars_, this);
    
    dispatcher_.start_event.subscribe(&Strategy::on_start_, this);
    
    dispatcher_.idle_event.subscribe(&Strategy::on_idle_, this);
    
    dispatcher_.stop_event.subscribe(&Strategy::on_stop_, this);
    
    // exchange now should be driven by feed events
//    dispatcher_.add_subject(exchange_);
    
    dispatcher_.add_subject(feed);
    
    log_trace("Strategy::{}", __func__);
}

void Strategy::run() 
{ 
    dispatcher_.run(); 
}

void Strategy::on_order_updated(order_t* order) 
{
    datetime_t datetime;
    size_t quantity = 0;
    real_t price = 0;
    order_info_t* info = order->info.get();
    if (info)
    {
        datetime = info->datetime;
        quantity = info->quantity;
        price = info->price;
        log_debug("Strategy::{} date={} action={} size={0.3f} price={0.3f}", __func__, to_str(datetime), order_t::to_str(order->action), quantity, price); 
    }
    else
        log_debug("Strategy::{0:} date={1:} action=%s size={2:0.3f} price={3:0.3f} (info is empty!)", __func__,
            papryka::to_str(get_current_datetime()), order_t::to_str(order->action), quantity, price);
}

void Strategy::on_bars_(const datetime_t& date, const values_t& values)
{
//#if defined(ENABLE_ANALYZERS)
//    analyzers_.before_on_bars(*this, values);
//#endif
    
    on_bars(date, values);
    
    bars_processed_event.emit(*this, values);
}

void Strategy::on_order_event_(Exchange& exchange, order_event_ptr_t orderEvent)
{
    assert (orderEvent.get() != nullptr);
    order_t* order = (order_t*)orderEvent->order;
    assert (order != nullptr);
    const uint32_t& order_id = order->id;
   
    log_debug("Strategy::{} {} order id={} order event type={}, order state={}", 
            __func__, to_str(orderEvent->datetime), order_id, order_t::Event::to_str(orderEvent->type), order_t::to_str(order->state));
    
    on_order_updated(order);
    
    order_to_position_t::iterator order_to_pos_iter = order_to_position_.find(order_id);
    if (order_to_pos_iter != order_to_position_.end())
    {
        const uint32_t& pos_id = order_to_pos_iter->second;
        assert (pos_id != 0);
        positions_t::iterator pos_iter = active_positions_.find(pos_id);       
        if (pos_iter != active_positions_.end())
        {
            position_t* pos = pos_iter->second.get();            
            assert (pos != nullptr);
            if (pos != nullptr)
            {   
                log_debug("Strategy::{} unlink order id={} from position id={}", __func__, order_id, pos_id);
                if (!order->is_active())
                    unregister_position_order(pos_id, order_id);
                
                assert (pos != nullptr);
                pos->on_order_event(orderEvent);
            }
        }
        else
        {
            log_warn("Strategy::{} position id={} is not on active list", __func__, pos_id);
        }
    }
    else
    {
        log_debug("Strategy::{} order id={} is not on active", __func__, order_id);
    }
}

void Strategy::register_position(position_ptr_t pos)
{
    assert (pos);
    const uint32_t& posId = pos->id;
    log_trace("Strategy::{} position id={}", __func__, posId);
    if (active_positions_.find(posId) == active_positions_.end())
        active_positions_.insert(typename positions_t::value_type(posId, pos));
    else
        log_warn("Strategy::{} position id={} already registered", __func__, posId);
}


void Strategy::unregister_position(uint32_t posId)
{
    log_trace("Strategy::{} position id={}", __func__, posId);
    assert (active_positions_.find(posId) != active_positions_.end());
    active_positions_.erase(posId);
}

void Strategy::register_position_order(uint32_t posId, uint32_t orderId)
{
    log_trace("Strategy::{} order id={} position id={}", __func__, orderId, posId);
    order_to_position_[orderId] = posId;
}

void Strategy::unregister_position_order(uint32_t posId, uint32_t orderId)
{
    assert(order_to_position_.find(orderId) != order_to_position_.end());
    order_to_position_.erase(orderId);
}

Strategy::position_t* Strategy::get_position(const uint32_t id)
{
    position_t* pos = nullptr;
    positions_t::iterator iter = active_positions_.find(id);
    if (iter != active_positions_.end())
        pos = iter->second.get();
    return pos;
}

real_t Strategy::get_result()
{
    return exchange->get_equity();
}

real_t Strategy::get_last_price(const std::string& symbol) const
{
    real_t ret = real_t(0.0);
    const value_t& value = feed->current_values[symbol];
    ret = value.close;
    return ret;
}

const datetime_t& Strategy::get_current_datetime() const 
{ 
    return feed->current_date;
}

Strategy::order_ptr_t Strategy::create_order(order_t::Type type, order_t::Action action, const std::string& symbol, size_t quantity, bool isFillOnClose, real_t stopPrice, real_t limitPrice)
{
    return exchange->create_order(type, action, symbol, quantity, isFillOnClose, stopPrice, limitPrice);            
}
    
Strategy::position_t* Strategy::enter_long(const std::string& symbol, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new LongPosition(*this, symbol, real_t(0), real_t(0), quantity, isGoodTillCanceled, isAllOrNone);
}

Strategy::position_t* Strategy::enter_short(const std::string& symbol, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new ShortPosition(*this, symbol, real_t(0), real_t(0), quantity, isGoodTillCanceled, isAllOrNone);
}


Strategy::position_t* Strategy::enter_long_stop(const std::string& symbol, size_t quantity, real_t stopPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new LongPosition(*this, symbol, stopPrice, real_t(0), quantity, isGoodTillCanceled, isAllOrNone);
}

Strategy::position_t* Strategy::enter_short_stop(const std::string& symbol, size_t quantity, real_t stopPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new ShortPosition(*this, symbol, stopPrice, real_t(0), quantity, isGoodTillCanceled, isAllOrNone);
}
   
Strategy::position_t* Strategy::enter_long_limit(const std::string& symbol, size_t quantity, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new LongPosition(*this, symbol, real_t(0), limitPrice, quantity, isGoodTillCanceled, isAllOrNone);
}

Strategy::position_t* Strategy::enter_short_limit(const std::string& symbol, size_t quantity, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new ShortPosition(*this, symbol, real_t(0), limitPrice, quantity, isGoodTillCanceled, isAllOrNone);
}

Strategy::position_t* Strategy::enter_long_stop_limit(const std::string& symbol, size_t quantity, real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new LongPosition(*this, symbol, stopPrice, limitPrice, quantity, isGoodTillCanceled, isAllOrNone);
}

Strategy::position_t* Strategy::enter_short_stop_limit(const std::string& symbol, size_t quantity, real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled, bool isAllOrNone)
{
    return new ShortPosition(*this, symbol, stopPrice, limitPrice, quantity, isGoodTillCanceled, isAllOrNone);
}





