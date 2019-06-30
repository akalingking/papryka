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
#include <cmath>

namespace papryka {
/**
 * @param _D    derived class
 * @param _T    bar type
 */
template <typename _D, typename _T=Bar>
class Strategy
{
public:
    class Position;

    typedef std::shared_ptr<Strategy<_D, _T>> ptr_t;
    typedef Exchange<_T> exchange_t;
    typedef typename exchange_t::value_t value_t;
    typedef typename exchange_t::values_t values_t;
    typedef typename exchange_t::feed_t feed_t;
    typedef typename exchange_t::feed_ptr_t feed_ptr_t;
    typedef typename exchange_t::order_t order_t;
    typedef typename exchange_t::order_ptr_t order_ptr_t;
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

    exchange_ptr_t exchange;
    feed_ptr_t feed;
    datetimes_t dates_;
    symbols_t symbols_;
    // used by analyzers and plots for observing incoming bars/values
    Event bars_processed_event;

    explicit Strategy(exchange_ptr_t exchange);
    ~Strategy();
    real_t get_result();
    real_t get_last_price(const std::string& symbol) const;
    const datetime_t& get_current_datetime() const;
    position_t* get_position(const uint32_t id);
    void run();

    // to be called only by position and derived classes
    void register_position(position_ptr_t pos);
    void unregister_position(uint32_t id);
    void register_position_order(uint32_t posId, uint32_t orderId);
    void unregister_position_order(uint32_t posId, uint32_t orderId);

    order_ptr_t create_order(typename order_t::Type type, typename order_t::Action action, const std::string& symbol, size_t quantity,
            bool isGoodTillCanceled=false, real_t stopPrice=real_t(0), real_t limitPrice=real_t(0));

    // arrg needs to write this as many, interface for custom strategies
    position_ptr_t enter_long(const std::string& symbol, size_t quantity, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    position_ptr_t enter_short(const std::string& symbol, size_t quantity, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    position_ptr_t enter_long_stop(const std::string& symbol,size_t quantity, real_t stopPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    position_ptr_t enter_short_stop(const std::string& symbol, size_t quantity, real_t stopPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    position_ptr_t enter_long_limit(const std::string& symbol, size_t quantity, real_t limitPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    position_ptr_t enter_short_limit(const std::string& symbol, size_t quantity, real_t limitPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    position_ptr_t enter_long_stop_limit(const std::string& symbol, size_t quantity, real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);
    position_ptr_t enter_short_stop_limit(const std::string& symbol, size_t quantity, real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled=false, bool isAllOrNone=false);

    // implemented by custom strategies, called from statemachine
    void on_enter(position_t* pos) { (static_cast<_D&>(*this)).on_enter(pos); };
    void on_enter_canceled(position_t* pos) { (static_cast<_D&>(*this)).on_enter_canceled(pos); };
    void on_exit(position_t* pos) { (static_cast<_D&>(*this)).on_exit(pos); };
    void on_exit_canceled(position_t* pos) { (static_cast<_D&>(*this)).on_exit_canceled(pos); };

    size_t active_position_size() const;
    size_t order_to_position_size() const;

protected:
    Dispatcher dispatcher_;
//    symbol_to_values_t prices_;
    order_to_position_t order_to_position_;
    positions_t active_positions_;

private:
    void on_order_event_(exchange_t& exchange, order_event_ptr_t orderEvent);
    void on_bars_(const datetime_t& date, const values_t& bars);
    void on_start_() { (static_cast<_D&>(*this)).on_start(); };
    void on_idle_() { (static_cast<_D&>(*this)).on_idle(); };
    void on_stop_() { (static_cast<_D&>(*this)).on_stop(); };
};

template <typename _D, typename _T>
class Strategy<_D, _T>::Position
{
public:
    typedef Strategy<_D,_T> strategy_t;
    // Implements statistics for position
    struct Tracker;
    // State transition logic
    struct StateMachine;
    // Internal string informations
    template<typename _U>
    struct statics_
    {
        static uint32_t id;
        static const char* directions[];
        static const char* states[];
    };

    typedef statics_<void> statics_t;
    typedef std::shared_ptr<Position> ptr_t;
    typedef Exchange<_T> exchange_t;
    typedef typename exchange_t::ptr_t exchange_ptr_t;
    typedef typename exchange_t::value_t value_t;
    typedef typename exchange_t::order_t order_t;
    typedef typename exchange_t::order_ptr_t order_ptr_t;
    typedef typename order_t::event_t order_event_t;
    typedef typename order_t::event_ptr_t order_event_ptr_t;
    typedef typename order_t::info_ptr_t order_info_ptr_t;
    typedef typename std::map<uint32_t, order_ptr_t> orders_t;

    enum State { StateIdle, StateOpen, StateClose };
    enum Direction { LongOnly, ShortOnly, LongShort };

    static const char* to_str(State e) { return statics_t::states[e]; }
    static const char* to_str(Direction e) { return statics_t::directions[e]; }

public:
    strategy_t& strategy;
    uint32_t id;
    Market::Type market_type;
    Direction direction;
    datetime_t entry_datetime;
    order_ptr_t entry_order;
    datetime_t exit_datetime;
    order_ptr_t exit_order;
    bool is_all_or_none;
    bool is_good_till_canceled;
    orders_t active_orders_;
    real_t shares_;

    virtual ~Position();
    const std::string& get_symbol() const;
    const Milliseconds get_age() const;
    real_t get_return(bool includeCommission = true) const;
    real_t get_pnl(bool includeCommission = true) const;
    real_t get_last_price() const;

    bool is_entry_active() const;
    bool is_entry_filled() const;
    bool is_exit_active() const;
    bool is_exit_filled() const;
    bool is_open() const;
    void cancel_entry();
    void cancel_exit();
    void exit_market(bool isGoodTillCanceled = false);
    void exit_limit(real_t limitPrice, bool isGoodTillCanceled = false);
    void exit_stop(real_t stopPrice, bool isGoodTillCanceled = false);
    void exit_stop_limit(real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled = false);

    void submit_exit_order(real_t stopPrice, real_t limitPrice, bool getGoodTillCancelled);
    void submit_and_register_order(order_ptr_t order);
    void on_order_event(order_event_ptr_t orderEvent);

protected:
    std::unique_ptr<Tracker> tracker_;
    std::unique_ptr<StateMachine> statemachine_;

    Position(Strategy& strategy, bool goodTillCanceled, bool isAllOrNone, Direction direction, Market::Type type = Market::Stock);
    void update_pos_tracker(order_event_t& orderEvent);
    virtual order_ptr_t build_exit_order(real_t stopPrice, real_t limitPrice) = 0;

private:
    statics_t statics_t_;
};

template <typename _D, typename _T>
class LongPosition : public Strategy<_D,_T>::Position
{
public:
    typedef Strategy<_D,_T> strategy_t;
    typedef typename strategy_t::Position base_t;
    typedef typename base_t::order_t order_t;
    typedef typename base_t::order_ptr_t order_ptr_t;

    LongPosition(strategy_t& strategy, const std::string& symbol, real_t stopPrice, real_t limitPrice, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone);

    ~LongPosition();

    order_ptr_t build_exit_order(real_t stopPrice, real_t limitPrice);
};

template <typename _D, typename _T>
class ShortPosition : public Strategy<_D,_T>::Position
{
public:
    typedef Strategy<_D,_T> strategy_t;
    typedef typename strategy_t::Position base_t;
    typedef typename base_t::order_t order_t;
    typedef typename base_t::order_ptr_t order_ptr_t;

    ShortPosition(strategy_t& strategy, const std::string& symbol,real_t stopPrice, real_t limitPrice, size_t quantity, bool isGoodTillCanceled, bool isAllOrNone);

    ~ShortPosition();

    order_ptr_t build_exit_order(real_t stopPrice, real_t limitPrice);
};

#include "impl/strategy.ipp"
#include "impl/position.ipp"
}
