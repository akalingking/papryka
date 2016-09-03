/**
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
 * 
 * @file        strategy.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        August 6, 2016 9:37 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com> 
 */
#pragma once
#include "../detail/types.h"
#include "../detail/event.h"
#include "../detail/date.h"
#include "../detail/dispatcher.h"
#include "../exchange/exchange.h"
#include "../exchange/market.h"
#include <memory>
#include <vector>
#include <map>

namespace papryka {

class Strategy
{
public:
    class Position;
    
    typedef std::shared_ptr<Strategy> ptr_t;
    typedef Exchange exchange_t;
    typedef typename exchange_t::value_t value_t;
    typedef typename exchange_t::values_t values_t;
    typedef typename Exchange::feed_t feed_t;
    typedef typename Exchange::feed_ptr_t feed_ptr_t;
    typedef typename Exchange::order_t order_t;
    typedef typename Exchange::order_ptr_t order_ptr_t;
    typedef typename order_t::event_t order_event_t;
    typedef typename order_t::event_ptr_t order_event_ptr_t;
    typedef typename order_t::info_t order_info_t;
    typedef typename order_t::info_ptr_t order_info_ptr_t;
    typedef Position position_t;
    typedef typename std::shared_ptr<position_t> position_ptr_t;
    typedef typename std::shared_ptr<exchange_t> exchange_ptr_t;
    typedef typename std::map<uint32_t, position_ptr_t>  positions_t;
    
    typedef std::map<uint32_t, uint32_t> order_to_position_t;
//    typedef std::vector<real_t> values_t;
    typedef std::vector<datetime_t> datetimes_t;
    typedef std::vector<std::string> symbols_t;
//    typedef std::map<std::string, values_t> symbol_to_values_t;
    
    exchange_ptr_t& exchange;
    feed_ptr_t& feed;
    datetimes_t dates_;
    symbols_t symbols_;
    // used by analyzers and plots for observing incoming bars/values
    Event bars_processed_event;
    
    inline explicit Strategy(exchange_ptr_t& exchange);
    inline real_t get_result();
    inline real_t get_last_price(const std::string& symbol) const;
    inline const datetime_t& get_current_datetime() const;
    inline position_t* get_position(const uint32_t id);
    inline void run();
    
    // to be called only by position and derived classes
    inline void register_position(position_ptr_t pos);
    inline void unregister_position(uint32_t id);
    inline void register_position_order(uint32_t posId, uint32_t orderId);
    inline void unregister_position_order(uint32_t posId, uint32_t orderId);
    inline order_ptr_t create_order(order_t::Type type, order_t::Action action, const std::string& symbol, size_t quantity, 
            bool isGoodTillCanceled=false, real_t stopPrice=real_t(0), real_t limitPrice=real_t(0));
    // arrg needs to write this as many, interface for custom strategies
    inline position_t* enter_long(const std::string& symbol, size_t quantity, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    inline position_t* enter_short(const std::string& symbol, size_t quantity, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    inline position_t* enter_long_stop(const std::string& symbol,size_t quantity, real_t stopPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    inline position_t* enter_short_stop(const std::string& symbol, size_t quantity, real_t stopPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    inline position_t* enter_long_limit(const std::string& symbol, size_t quantity, real_t limitPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    inline position_t* enter_short_limit(const std::string& symbol, size_t quantity, real_t limitPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    inline position_t* enter_long_stop_limit(const std::string& symbol, size_t quantity, real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    inline position_t* enter_short_stop_limit(const std::string& symbol, size_t quantity, real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    
    // implemented by custom strategies
    virtual void on_enter(position_t* pos) {};
    virtual void on_enter_canceled(position_t* pos) {};
    virtual void on_exit(position_t* pos) {};
    virtual void on_exit_canceled(position_t* pos) {};
    
protected:
    positions_t active_positions_;
    Dispatcher dispatcher_;
//    symbol_to_values_t prices_;
    order_to_position_t order_to_position_;
    // to be implemented by custom strategies
    virtual void on_start() {}
    virtual void on_idle() {}
    virtual void on_stop() {}
    inline virtual void on_order_updated(order_t* order);
    virtual void on_bars(const datetime_t& date, const values_t& bars) = 0;
    
private:
    inline void on_order_event_(exchange_t& exchange, order_event_ptr_t orderEvent);
    inline void on_bars_(const datetime_t& date, const values_t& bars);
    void on_start_() { on_start(); };
    void on_idle_() { on_idle(); };
    void on_stop_() { on_stop(); };
};

class Strategy::Position
{
public:
    // Implements statistics for position
    struct Tracker;
    // State transition logic
    struct StateMachine;
    // Internal string informations
    template<typename _T> 
    struct statics_  { 
        static uint32_t id;
        static const char* directions[];
        static const char* states[];
    };
    
    typedef statics_<void> statics_t;
    typedef std::shared_ptr<Position> ptr_t;
    typedef Exchange exchange_t;
    typedef typename exchange_t::ptr_t exchange_ptr_t;
    typedef typename exchange_t::value_t value_t;
    typedef typename exchange_t::order_t order_t;
    typedef typename exchange_t::order_ptr_t order_ptr_t;
    typedef typename order_t::event_t order_event_t;
    typedef typename order_t::event_ptr_t order_event_ptr_t;
    typedef typename order_t::info_ptr_t order_info_ptr_t;
    typedef typename std::map<uint32_t, order_ptr_t> orders_t;
    
    enum State { StateIdle=1, StateOpen=2, StateClose=3 };
    enum Direction { LongOnly=1, ShortOnly=2, LongShort=3 };
    
    static const char* to_str(State e) { return statics_t::states[e]; }
    static const char* to_str(Direction e) { return statics_t::directions[e]; }

public:
    Strategy& strategy;
    uint32_t id;
    Market::Type market_type;
    Direction direction;
    datetime_t entry_datetime;
    order_ptr_t entry_order;
    datetime_t exit_datetime;
    order_ptr_t exit_order;
    bool is_all_or_none;
    bool is_good_till_canceled;

    inline virtual ~Position();
    inline const std::string& get_symbol() const;
    inline const Milliseconds get_age() const;
    inline real_t get_return(bool includeCommission = true) const;
    inline real_t get_pnl(bool includeCommission = true) const;
    inline real_t get_last_price() const;
    
    inline bool is_entry_active() const;
    inline bool is_entry_filled() const;
    inline bool is_exit_active() const;
    inline bool is_exit_filled() const;
    inline bool is_open() const;
    inline void cancel_entry();
    inline void cancel_exit();
    inline void exit_market(bool isGoodTillCanceled = false);
    inline void exit_limit(real_t limitPrice, bool isGoodTillCanceled = false);
    inline void exit_stop(real_t stopPrice, bool isGoodTillCanceled = false);
    inline void exit_stop_limit(real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled = false);
    
    inline void submit_exit_order(real_t stopPrice, real_t limitPrice, bool getGoodTillCancelled);
    inline void submit_and_register_order(order_ptr_t order);
    inline void on_order_event(order_event_ptr_t orderEvent);

protected:
    orders_t active_orders_;
    std::unique_ptr<Tracker> tracker_;
    std::unique_ptr<StateMachine> statemachine_;
    real_t shares_;
    
    inline Position(Strategy& strategy, bool goodTillCanceled, bool isAllOrNone, Direction direction, Market::Type type = Market::Stock);    
    inline void update_pos_tracker(order_event_t& orderEvent);
    virtual order_ptr_t build_exit_order(real_t stopPrice, real_t limitPrice) = 0;
    
private:
    statics_t statics_t_;
};

class LongPosition : public Strategy::Position
{
public:
    inline LongPosition(Strategy& strategy, const std::string& symbol, real_t stopPrice, real_t limitPrice, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone);
    inline ~LongPosition();
    inline order_ptr_t build_exit_order(real_t stopPrice, real_t limitPrice);
};

class ShortPosition : public Strategy::Position
{
public:
    inline ShortPosition(Strategy& strategy, const std::string& symbol,real_t stopPrice, real_t limitPrice, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone);
    inline ~ShortPosition();
    inline order_ptr_t build_exit_order(real_t stopPrice, real_t limitPrice);
};

#include "impl/strategy.ipp"
#include "impl/position.ipp"
}