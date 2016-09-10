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
class Strategy<_D,_T>::Position::Tracker
{
public:
    inline Tracker();
    real_t get_shares() const { return shares_; }
    inline real_t get_cash() const;
    real_t get_cost_basis() const { return cost_basis_; }
    real_t get_commissions() const { return commissions_; }
    inline real_t get_net_profit(real_t price, bool includeCommission=true) const;
    inline real_t get_return(real_t price, bool includeCommission=true) const;
    inline void buy(size_t quantity, real_t price, real_t commission);
    inline void sell(size_t quantity, real_t price, real_t commission);
    inline void reset();
        
private:
    inline void update_(size_t quantity, real_t price, real_t commission);
    real_t shares_;
    real_t cash_;
    real_t commissions_;
    real_t cost_basis_;
    real_t cost_per_share_; // volume weighted ave price per share.
};

template <typename _D, typename _T>
Strategy<_D,_T>::Position::Tracker::Tracker()
{ reset(); }

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::Tracker::reset()
{
    shares_ = real_t(0);
    cash_ = real_t(0.0);
    commissions_ = real_t(0.0);
    cost_basis_ = real_t(0.0);
    cost_per_share_ = real_t(0.0);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::Tracker::update_(size_t quantity, real_t price, real_t commission)
{
    assert (quantity != 0);
    size_t total_shares = 0;
    if (shares_ == 0)
    {
        // Opening new position
        total_shares = quantity;
        cost_per_share_ = price;
        cost_basis_ = abs(quantity) * price;
    }
    else
    {
        total_shares = precision::round(shares_+quantity);
        if (total_shares != 0)
        {
            // copysign
            size_t prev_direction = (shares_ > 0) ? 1.0 :-1.0;
            size_t txnDirection = (quantity > 0) ? 1.0 : -1.0;
            
            if (prev_direction != txnDirection)
            {
                if (abs(quantity) > abs(shares_))
                {
                    // Going from Long to short or vice versa
                    // Update cost as a new position is being opened.
                    cost_per_share_ = price;
                    size_t diff = precision::round(shares_ + quantity);
                    cost_basis_ = abs(diff) * price;
                }
                else
                {
                    // Reducing position
                    ;
                }
            }
            else
            {
                // Increasing the current position
                // Calculate volume weighted ave price per share
                size_t prev_cost = cost_per_share_ * shares_;
                size_t txn_cost = quantity * price;
                cost_per_share_ = (prev_cost + txn_cost) / total_shares;
                cost_basis_ += abs(quantity) * price;
            }
        }
        else
        {
            // Closing positions
            cost_per_share_ = 0.0;
        }
    }
    cash_ += price * quantity * -1;
    commissions_ += commission;
    shares_ = total_shares;
}

template <typename _D, typename _T>
real_t Strategy<_D,_T>::Position::Tracker::get_cash() const
{
    return cash_ - commissions_;
}

template <typename _D, typename _T>
real_t Strategy<_D,_T>::Position::Tracker::get_net_profit(real_t price, bool includeCommission) const
{
    real_t ret = cash_;
    if (price == 0)
        price = cost_per_share_;
    ret += price * shares_;
    if (includeCommission)
        ret -= commissions_;
    return ret;
}

template <typename _D, typename _T>
real_t Strategy<_D,_T>::Position::Tracker::get_return(real_t price, bool includeCommission) const
{
    real_t ret = 0.0;
    real_t net_profit = get_net_profit(price, includeCommission);
    if (cost_basis_ != 0)
        ret = net_profit / cost_basis_;
    return ret;
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::Tracker::buy(size_t quantity, real_t price, real_t commission)
{
    assert (quantity > 0);
    this->update_(quantity, price, commission);   
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::Tracker::sell(size_t quantity, real_t price, real_t commission)
{
    assert (quantity > 0);
    update_(quantity*-1.0, price, commission);
}


template <typename _D, typename _T>
struct Strategy<_D,_T>::Position::StateMachine
{
    Position& position;
    State state;
    
    inline StateMachine(Position& position);
    inline void switch_state(State newState);
    inline bool is_open() const;
    inline bool can_submit_order(order_t& order);
    inline void on_order_event(order_event_t& event);
    inline void enter();
    inline void exit(real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled);
};
#include "statemachine.ipp"
    
template <typename _D, typename _T> template<typename _U> uint32_t Strategy<_D,_T>::Position::statics_<_U>::id = 0;
template <typename _D, typename _T> template<typename _U> const char*  Strategy<_D,_T>::Position::statics_<_U>::directions[] = {"LongOnly","ShortOnly","LongShort"};
template <typename _D, typename _T> template<typename _U> const char* Strategy<_D,_T>::Position::statics_<_U>::states[] = {"StateIdle","StateOpen","StateClose"};

template <typename _D, typename _T>
Strategy<_D,_T>::Position::Position(Strategy& strategy, bool isGoodTillCanceled, bool isAllOrNone, Direction direction, Market::Type marketType) :
    strategy(strategy), market_type(marketType), direction(direction), is_good_till_canceled(isGoodTillCanceled), is_all_or_none(isAllOrNone),
    shares_(0), id(++statics_t::id), statemachine_(new StateMachine(*this)), tracker_(new Tracker())
{
    log_trace("Position::{} id={}",  __func__, id);
}

template <typename _D, typename _T>
Strategy<_D,_T>::Position::~Position()
{
    log_debug("Position::{} id={}", __func__, id);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::submit_and_register_order(order_ptr_t order)
{
    if (!statemachine_->can_submit_order(*order.get())) {
        assert(false);
        return;
    }
    
    strategy.exchange->submit_order(order);
    strategy.register_position_order(id, order->id);
    active_orders_[order->id] = order;
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::submit_exit_order(real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled)
{
    assert (is_entry_active());
    assert (!is_exit_active());
    
    exit_order = build_exit_order(stopPrice, limitPrice);
    isGoodTillCanceled = entry_order->is_good_till_canceled;
    exit_order->is_good_till_canceled = isGoodTillCanceled;
    exit_order->is_all_or_none = is_all_or_none;
    
    submit_and_register_order(exit_order);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::on_order_event(order_event_ptr_t orderEvent)
{   
    log_trace("Position:{} {} id={} event={}", __func__,
        papryka::to_str(orderEvent->datetime), id, order_t::Event::to_str(orderEvent->type));
    
    assert (statemachine_->state);
    assert (orderEvent->order);
    
//    update_pos_tracker_(*orderEvent);
    order_t* order = (order_t*)orderEvent->order;
    
    if (!order->is_active())
    {
        // Do not erase since we will remove the pointer
        // Todo transfer it to another map
        // Exchange and position must hold unique copies of order pointers
        // Position hold the pointers
        // Exchange must generate order id's
        // active_orders_.erase(order->id());
        // remark this for now
        active_orders_.erase(order->id);
    }
    
    if (orderEvent->type == order_t::Event::Filled || orderEvent->type == order_t::Event::PartiallyFilled)
    {
        order_info_t* info = orderEvent->info.get();
        assert (info);
        if (order->is_buy())
            shares_ += precision::round(info->quantity);
        else
            shares_ -= precision::round(info->quantity);

        log_debug("Position::{0:} {1:} id={2:} event={3:} order price={4:} order size={5:0.3f} current share(s)={6:0.3f}", 
                __func__, papryka::to_str(orderEvent->datetime), id, order_t::Event::to_str(orderEvent->type), info->price, info->quantity, shares_);
    }
    
    statemachine_->on_order_event(*orderEvent.get());
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::update_pos_tracker(order_event_t& orderEvent)
{   
    if (orderEvent.type == order_t::Event::PartiallyFilled || orderEvent.type  == order_t::Event::Filled)
    {
        order_t* order = (order_t*)orderEvent.order;
        order_info_t* info = orderEvent.info.get();
        if (order && info) 
        {
            if (order->is_buy())
                tracker_->buy(info->quantity, info->price, info->commission);
            else
                tracker_->sell(info->quantity, info->price, info->commission);
        }
    }
}

template <typename _D, typename _T>
bool Strategy<_D,_T>::Position::is_entry_active() const 
{ 
    return (entry_order && entry_order->is_active());
}

template <typename _D, typename _T>
bool Strategy<_D,_T>::Position::is_entry_filled() const 
{   
    return (entry_order && entry_order->is_filled());
}

template <typename _D, typename _T>
bool Strategy<_D,_T>::Position::is_exit_active() const 
{
    return (exit_order && exit_order->is_active());
}

template <typename _D, typename _T>
bool Strategy<_D,_T>::Position::is_exit_filled() const 
{ 
    return (exit_order && exit_order->is_filled()); 
}

template <typename _D, typename _T>
bool Strategy<_D,_T>::Position::is_open() const
{
    return statemachine_->is_open();
}

template <typename _D, typename _T>
const std::string& Strategy<_D,_T>::Position::get_symbol() const 
{ 
    return entry_order->symbol; 
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::cancel_entry()
{
    if (entry_order && entry_order->is_active())
        strategy.exchange->cancel_order(entry_order->id);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::cancel_exit()
{
    if (exit_order && exit_order->is_active())
        strategy.exchange->cancel_order(exit_order->id);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::exit_market(bool goodTillCanceled) 
{ 
    statemachine_->exit(0, 0, goodTillCanceled);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::exit_limit(real_t limitPrice, bool goodTillCanceled)
{
    statemachine_->exit(0, limitPrice, goodTillCanceled);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::exit_stop(real_t stopPrice, bool goodTillCanceled)
{
    statemachine_->exit(stopPrice, 0, goodTillCanceled);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::exit_stop_limit(real_t stopPrice, real_t limitPrice, bool goodTillCanceled)
{
    statemachine_->exit(stopPrice, limitPrice, goodTillCanceled);
}

template <typename _D, typename _T>
real_t Strategy<_D,_T>::Position::get_last_price() const
{
    real_t ret = 0;
    std::string symbol = get_symbol();
    if (!symbol.empty())
        ret = strategy.feed->current_values[get_symbol()].close;
    return ret;
}

template <typename _D, typename _T>
real_t Strategy<_D,_T>::Position::get_return(bool includeCommission) const
{
    log_trace("Position:{0:} last price {1:0.3f}", __func__, get_last_price());
    
    real_t ret = 0;
    real_t price = get_last_price();
    if (price != 0)
    {
        log_debug("Position::{} from tracker {0.3f}", __func__, tracker_->get_return(price, includeCommission));
        ret = tracker_->get_return(price, includeCommission);
    }
    return ret;
}

template <typename _D, typename _T>
real_t Strategy<_D,_T>::Position::get_pnl(bool includeCommission) const
{
    real_t ret = 0;
    real_t price = get_last_price();
    if (price != 0)
        ret = tracker_->get_net_profit(price, includeCommission);
    return ret;
}

template <typename _D, typename _T>
const Milliseconds Strategy<_D,_T>::Position::get_age() const
{
    Milliseconds age = Milliseconds(0);
    if (entry_datetime != nulldate) 
    {
        datetime_t last = (exit_datetime != nulldate) ? exit_datetime : strategy.get_current_datetime();    
        age = std::chrono::duration_cast<Milliseconds>(last - entry_datetime);
    }
    return age;
}
//    
template <typename _D, typename _T>    
LongPosition<_D,_T>::LongPosition(strategy_t& strategy, const std::string& symbol, real_t stopPrice, real_t limitPrice, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone) : 
            base_t(strategy, isGoodTillCanceled, isAllOrNone, base_t::Position::LongOnly, Market::Stock)
{
    assert (quantity > 0);
    if (limitPrice == 0 && stopPrice == 0)
    {
        base_t::entry_order = strategy.create_order(order_t::Market, order_t::Action::Buy, symbol, quantity,  false);
    }
    else if (limitPrice != 0 && stopPrice == 0)
    {
        base_t::entry_order = strategy.create_order(order_t::Limit, order_t::Action::Buy, symbol, quantity, false, limitPrice);
    }
    else if (limitPrice == 0 && stopPrice != 0)
    {
        base_t::entry_order = strategy.create_order(order_t::Stop, order_t::Action::Buy, symbol, quantity, false, 0, stopPrice);
    }
    else if (limitPrice != 0 && stopPrice != 0)
    {
        base_t::entry_order = strategy.create_order(order_t::StopLimit, order_t::Action::Buy, symbol, quantity, false, limitPrice, stopPrice);
    }
    else
    {
        assert (false);
    }
    
    base_t::entry_order->is_all_or_none = isAllOrNone;
    base_t::entry_order->is_good_till_canceled = isGoodTillCanceled;
    
    base_t::tracker_.reset(new typename base_t::Tracker());
    
    base_t::submit_and_register_order(base_t::entry_order);
    strategy.register_position(base_t::Position::ptr_t(this));
    
    base_t::statemachine_->switch_state(base_t::StateIdle);
    
    log_trace("LongPosition::{} id={}", __func__, base_t::id);
}

template <typename _D, typename _T>
LongPosition<_D,_T>::~LongPosition()
{
    log_trace("LongPositin::{} id={}", __func__, base_t::id);
}

template <typename _D, typename _T>
typename LongPosition<_D,_T>::order_ptr_t LongPosition<_D,_T>::build_exit_order(real_t stopPrice, real_t limitPrice)
{
    log_debug("LongPosition::{} id={} stop={} limit={}", __func__, base_t::id, stopPrice, limitPrice);
    
    std::string symbol = base_t::get_symbol();
    
    size_t quantity = base_t::shares_;
    
    order_ptr_t order;
    if (limitPrice == 0 && stopPrice == 0)
    {
        order = base_t::strategy.exchange->create_order(order_t::Market, order_t::Sell, symbol, quantity,  false);
    }
    else if (limitPrice != 0 && stopPrice == 0)
    {
        order = base_t::strategy.exchange->create_order(order_t::Limit, order_t::Action::Sell, symbol, quantity, false, limitPrice);
    }
    else if (limitPrice == 0 && stopPrice != 0)
    {
        order = base_t::strategy.exchange->create_order(order_t::Stop, order_t::Sell, symbol, quantity, false, real_t(0), stopPrice);
    }
    else if (limitPrice != 0 && stopPrice != 0)
    {
        order = base_t::strategy.exchange->create_order(order_t::StopLimit, order_t::Sell, symbol, quantity, false, limitPrice, stopPrice);
    }
    else
        assert (false);
    
    return order;
}

template <typename _D, typename _T>
ShortPosition<_D,_T>::ShortPosition(strategy_t& strategy, const std::string& symbol, real_t stopPrice, real_t limitPrice, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone) :
    base_t::Position(strategy, isGoodTillCanceled, isAllOrNone, Strategy<_D,_T>::Position::ShortOnly, Market::Stock)
{
    if (limitPrice == 0 && stopPrice == 0)
    {
        this->entry_order = this->strategy.exchange->create_order(order_t::Market, order_t::SellShort, symbol, quantity,  false);
    }
    else if (limitPrice != 0 && stopPrice == 0)
    {
        this->entry_order = this->strategy.exchange->create_order(order_t::Limit, order_t::SellShort, symbol, quantity, false, limitPrice);
    }
    else if (limitPrice == 0 && stopPrice != 0)
    {
        this->entry_order = this->strategy.exchange->create_order(order_t::Stop, order_t::SellShort, symbol, quantity, false,real_t(0), stopPrice);
    }
    else if (limitPrice != 0 && stopPrice != 0)
    {
        this->entry_order = this->strategy.exchange->create_order(order_t::StopLimit, order_t::SellShort, symbol, quantity, false, limitPrice, stopPrice);
    }
    else
        assert (false);
    
    this->entry_order->is_all_or_none = isAllOrNone;
    this->entry_order->is_good_till_canceled = isGoodTillCanceled;
    
    this->submit_and_register_order(this->entry_order);
    this->strategy.register_position(base_t::Position::ptr_t(this));
    
    this->statemachine_->switch_state(base_t::StateIdle);
    
    log_trace("ShortPosition::{} id={}", __func__, this->id);
}

template <typename _D, typename _T>
ShortPosition<_D,_T>::~ShortPosition()
{
    log_trace("ShortPosition::{} id={}", __func__, this->id);
}

template <typename _D, typename _T>
typename ShortPosition<_D,_T>::order_ptr_t ShortPosition<_D,_T>::build_exit_order(real_t stopPrice, real_t limitPrice)
{
    log_trace("ShortPosition<_D,_T>::{0:} id={} stop={} limit={}", __func__, this->id, stopPrice, limitPrice);
    
    std::string symbol = this->get_symbol();
    size_t quantity = this->shares_ * -1;
    
    order_ptr_t order;
    if (limitPrice == 0 && stopPrice == 0)
    {
        order = this->strategy.exchange->create_order(order_t::Market, order_t::BuyToCover, symbol, quantity,  false);
    }
    else if (limitPrice != 0 && stopPrice == 0)
    {
        order = this->strategy.exchange->create_order(order_t::Limit, order_t::BuyToCover, symbol, quantity, limitPrice);
    }
    else if (limitPrice == 0 && stopPrice != 0)
    {
        order = this->strategy.exchange->create_order(order_t::Stop, order_t::BuyToCover, symbol, quantity, stopPrice);
    }
    else if (limitPrice != 0 && stopPrice != 0)
    {
        order = this->strategy.exchange->create_order(order_t::StopLimit, order_t::BuyToCover, symbol, quantity, stopPrice, limitPrice);
    }
    else
        assert (false);
    
    return order;
}




