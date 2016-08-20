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
class Strategy::Position::Tracker
{
public:
    Tracker();
    float get_shares() const { return shares_; }
    float get_cash() const;
    float get_cost_basis() const { return cost_basis_; }
    float get_commissions() const { return commissions_; }
    float get_net_profit(float price, bool includeCommission=true) const;
    float get_return(float price, bool includeCommission=true) const;
    void buy(float quantity, float price, float commission);
    void sell(float quantity, float price, float commission);
    void reset();
        
private:
    void update_(float quantity, float price, float commission);
    float shares_;
    float cash_;
    float commissions_;
    float cost_basis_;
    float cost_per_share_; // volume weighted ave price per share.
};

Strategy::Position::Tracker::Tracker()
{ reset(); }

void Strategy::Position::Tracker::reset()
{
    shares_ = 0;
    cash_ = 0.0;
    commissions_ = 0.0;
    cost_basis_ = 0.0;
    cost_per_share_ = 0.0;
}

void Strategy::Position::Tracker::update_(float quantity, float price, float commission)
{
    assert (quantity != 0);
    float total_shares = 0;
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
            float prev_direction = (shares_ > 0) ? 1.0 :-1.0;
            float txnDirection = (quantity > 0) ? 1.0 : -1.0;
            
            if (prev_direction != txnDirection)
            {
                if (abs(quantity) > abs(shares_))
                {
                    // Going from Long to short or vice versa
                    // Update cost as a new position is being opened.
                    cost_per_share_ = price;
                    float diff = precision::round(shares_ + quantity);
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
                float prev_cost = cost_per_share_ * shares_;
                float txn_cost = quantity * price;
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

float Strategy::Position::Tracker::get_cash() const
{
    return cash_ - commissions_;
}

float Strategy::Position::Tracker::get_net_profit(float price, bool includeCommission) const
{
    float ret = cash_;
    if (price == 0)
        price = cost_per_share_;
    ret += price * shares_;
    if (includeCommission)
        ret -= commissions_;
    return ret;
}

float Strategy::Position::Tracker::get_return(float price, bool includeCommission) const
{
    float ret = 0.0;
    float net_profit = get_net_profit(price, includeCommission);
    if (cost_basis_ != 0)
        ret = net_profit / cost_basis_;
    return ret;
}

void Strategy::Position::Tracker::buy(float quantity, float price, float commission)
{
    assert (quantity > 0);
    this->update_(quantity, price, commission);   
}

void Strategy::Position::Tracker::sell(float quantity, float price, float commission)
{
    assert (quantity > 0);
    update_(quantity*-1.0, price, commission);
}



struct Strategy::Position::StateMachine
{
    Position& position;
    State state;
    StateMachine(Position& position);
    void switch_state(State newState);
    bool is_open() const;
    bool can_submit_order(order_t& order);
    void on_order_event(order_event_t& event);
    void enter();
    void exit(float stopPrice, float limitPrice, bool isGoodTillCanceled);
};
#include "statemachine.ipp"
    
template<typename _T> uint32_t Strategy::Position::statics_<_T>::id = 0;
template<typename _T> const char*  Strategy::Position::statics_<_T>::directions[] = {"LongOnly","ShortOnly","LongShort"};
template<typename _T> const char* Strategy::Position::statics_<_T>::states[] = {"StateIdle","StateOpen","StateClose"};

Strategy::Position::Position(Strategy& strategy, bool isGoodTillCanceled, bool isAllOrNone, Direction direction, Market::Type marketType) :
    strategy(strategy), market_type(marketType), direction(direction), is_good_till_canceled(isGoodTillCanceled), is_all_or_none(isAllOrNone),
    shares_(0), id(++statics_t::id), statemachine_(new StateMachine(*this)), tracker_(new Tracker())
{
    log_trace("Position::{} id={}",  __func__, id);
}

Strategy::Position::~Position()
{
    log_debug("Position::{} id={}", __func__, id);
}

void Strategy::Position::submit_and_register_order(order_ptr_t order)
{
    if (!statemachine_->can_submit_order(*order.get())) {
        assert(false);
        return;
    }
    
    strategy.exchange->submit_order(order);
    strategy.register_position_order(id, order->id);
    active_orders_[order->id] = order;
}

void Strategy::Position::submit_exit_order(float stopPrice, float limitPrice, bool isGoodTillCanceled)
{
    assert (is_entry_active());
    assert (!is_exit_active());
    
    exit_order = build_exit_order(stopPrice, limitPrice);
    isGoodTillCanceled = entry_order->is_good_till_canceled;
    exit_order->is_good_till_canceled = isGoodTillCanceled;
    exit_order->is_all_or_none = is_all_or_none;
    
    submit_and_register_order(exit_order);
}

void Strategy::Position::on_order_event(order_event_ptr_t orderEvent)
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

void Strategy::Position::update_pos_tracker(order_event_t& orderEvent)
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

bool Strategy::Position::is_entry_active() const 
{ 
    return (entry_order && entry_order->is_active());
}

bool Strategy::Position::is_entry_filled() const 
{   
    return (entry_order && entry_order->is_filled());
}

bool Strategy::Position::is_exit_active() const 
{
    return (exit_order && exit_order->is_active());
}

bool Strategy::Position::is_exit_filled() const 
{ 
    return (exit_order && exit_order->is_filled()); 
}

bool Strategy::Position::is_open() const
{
    return statemachine_->is_open();
}

const std::string& Strategy::Position::get_symbol() const 
{ 
    return entry_order->symbol; 
}

void Strategy::Position::cancel_entry()
{
    if (entry_order && entry_order->is_active())
        strategy.exchange->cancel_order(entry_order->id);
}

void Strategy::Position::cancel_exit()
{
    if (exit_order && exit_order->is_active())
        strategy.exchange->cancel_order(exit_order->id);
}

void Strategy::Position::exit_market(bool goodTillCanceled) 
{ 
    statemachine_->exit(0, 0, goodTillCanceled);
}

void Strategy::Position::exit_limit(float limitPrice, bool goodTillCanceled)
{
    statemachine_->exit(0, limitPrice, goodTillCanceled);
}

void Strategy::Position::exit_stop(float stopPrice, bool goodTillCanceled)
{
    statemachine_->exit(stopPrice, 0, goodTillCanceled);
}

void Strategy::Position::exit_stop_limit(float stopPrice, float limitPrice, bool goodTillCanceled)
{
    statemachine_->exit(stopPrice, limitPrice, goodTillCanceled);
}

float Strategy::Position::get_last_price() const
{
    float ret = 0;
    std::string symbol = get_symbol();
    if (!symbol.empty())
        ret = strategy.feed->current_values[get_symbol()].close;
    return ret;
}

float Strategy::Position::get_return(bool includeCommission) const
{
    log_trace("Position:{0:} last price {1:0.3f}", __func__, get_last_price());
    
    float ret = 0;
    float price = get_last_price();
    if (price != 0)
    {
        log_debug("Position::{} from tracker {0.3f}", __func__, tracker_->get_return(price, includeCommission));
        ret = tracker_->get_return(price, includeCommission);
    }
    return ret;
}

float Strategy::Position::get_pnl(bool includeCommission) const
{
    float ret = 0;
    float price = get_last_price();
    if (price != 0)
        ret = tracker_->get_net_profit(price, includeCommission);
    return ret;
}

const Milliseconds Strategy::Position::get_age() const
{
    Milliseconds age = Milliseconds(0);
    if (entry_datetime != nulldate) 
    {
        datetime_t last = (exit_datetime != nulldate) ? exit_datetime : strategy.get_current_datetime();    
        age = std::chrono::duration_cast<Milliseconds>(last - entry_datetime);
    }
    return age;
}
    
    
LongPosition::LongPosition(Strategy& strategy, const std::string& symbol, float stopPrice, float limitPrice, int quantity,
        bool isGoodTillCanceled, bool isAllOrNone) : Position(strategy, isGoodTillCanceled, isAllOrNone, Position::LongOnly, Market::Stock)
{
    assert (quantity > 0);
    if (limitPrice == 0 && stopPrice == 0)
    {
        entry_order = strategy.create_order(order_t::Market, order_t::Action::Buy, symbol, quantity,  false);
    }
    else if (limitPrice != 0 && stopPrice == 0)
    {
        entry_order = strategy.create_order(order_t::Limit, order_t::Action::Buy, symbol, quantity, false, limitPrice);
    }
    else if (limitPrice == 0 && stopPrice != 0)
    {
        entry_order = strategy.create_order(order_t::Stop, order_t::Action::Buy, symbol, quantity, false, 0, stopPrice);
    }
    else if (limitPrice != 0 && stopPrice != 0)
    {
        entry_order = strategy.create_order(order_t::StopLimit, order_t::Action::Buy, symbol, quantity, false, limitPrice, stopPrice);
    }
    else
    {
        assert (false);
    }
    
    entry_order->is_all_or_none = isAllOrNone;
    entry_order->is_good_till_canceled = isGoodTillCanceled;
    
    tracker_.reset(new Tracker());
    
    submit_and_register_order(entry_order);
    strategy.register_position(Position::ptr_t(this));
    
    statemachine_->switch_state(StateIdle);
    
    log_trace("LongPosition::{} id={}", __func__, id);
}

LongPosition::~LongPosition()
{
    log_trace("LongPositin::{} id={}", __func__, id);
}

Strategy::Position::order_ptr_t LongPosition::build_exit_order(float stopPrice, float limitPrice)
{
    log_debug("LongPosition::{0:} id={1:} stop={2:0.3f} limit={3:0.3f}", __func__, id, stopPrice, limitPrice);
    
    std::string symbol = get_symbol();
    
    int quantity = shares_;
    
    order_ptr_t order;
    if (limitPrice == 0 && stopPrice == 0)
    {
        order = strategy.exchange->create_order(order_t::Market, order_t::Sell, symbol, quantity,  false);
    }
    else if (limitPrice != 0 && stopPrice == 0)
    {
        order = strategy.exchange->create_order(order_t::Limit, order_t::Action::Sell, symbol, quantity, false, limitPrice);
    }
    else if (limitPrice == 0 && stopPrice != 0)
    {
        order = strategy.exchange->create_order(order_t::Stop, order_t::Sell, symbol, quantity, false, 0, stopPrice);
    }
    else if (limitPrice != 0 && stopPrice != 0)
    {
        order = strategy.exchange->create_order(order_t::StopLimit, order_t::Sell, symbol, quantity, false, limitPrice, stopPrice);
    }
    else
        assert (false);
    
    return order;
}

ShortPosition::ShortPosition(Strategy& strategy, const std::string& symbol, float stopPrice, float limitPrice, int quantity, bool isGoodTillCanceled, bool isAllOrNone) :
    Position(strategy, isGoodTillCanceled, isAllOrNone, Strategy::Position::ShortOnly, Market::Stock)
{
    if (limitPrice == 0 && stopPrice == 0)
    {
        entry_order = strategy.exchange->create_order(order_t::Market, order_t::SellShort, symbol, quantity,  false);
    }
    else if (limitPrice != 0 && stopPrice == 0)
    {
        entry_order = strategy.exchange->create_order(order_t::Limit, order_t::SellShort, symbol, quantity, false, limitPrice);
    }
    else if (limitPrice == 0 && stopPrice != 0)
    {
        entry_order = strategy.exchange->create_order(order_t::Stop, order_t::SellShort, symbol, quantity, false, 0, stopPrice);
    }
    else if (limitPrice != 0 && stopPrice != 0)
    {
        entry_order = strategy.exchange->create_order(order_t::StopLimit, order_t::SellShort, symbol, quantity, false, limitPrice, stopPrice);
    }
    else
        assert (false);
    
    entry_order->is_all_or_none = isAllOrNone;
    entry_order->is_good_till_canceled = isGoodTillCanceled;
    
    submit_and_register_order(entry_order);
    strategy.register_position(Position::ptr_t(this));
    
    statemachine_->switch_state(StateIdle);
    
    log_trace("ShortPosition::{} id={}", __func__, id);
}

ShortPosition::~ShortPosition()
{
    log_trace("ShortPosition::{} id={}", __func__, id);
}

Strategy::Position::order_ptr_t ShortPosition::build_exit_order(float stopPrice, float limitPrice)
{
    log_trace("ShortPosition::{0:} id={1:} stop={2:0.3f} limit={3:0.3f}", __func__, id, stopPrice, limitPrice);
    
    std::string symbol = get_symbol();
    int quantity = shares_ * -1;
    
    order_ptr_t order;
    if (limitPrice == 0 && stopPrice == 0)
    {
        order = strategy.exchange->create_order(order_t::Market, order_t::BuyToCover, symbol, quantity,  false);
    }
    else if (limitPrice != 0 && stopPrice == 0)
    {
        order = strategy.exchange->create_order(order_t::Limit, order_t::BuyToCover, symbol, quantity, limitPrice);
    }
    else if (limitPrice == 0 && stopPrice != 0)
    {
        order = strategy.exchange->create_order(order_t::Stop, order_t::BuyToCover, symbol, quantity, stopPrice);
    }
    else if (limitPrice != 0 && stopPrice != 0)
    {
        order = strategy.exchange->create_order(order_t::StopLimit, order_t::BuyToCover, symbol, quantity, stopPrice, limitPrice);
    }
    else
        assert (false);
    
    return order;
}




