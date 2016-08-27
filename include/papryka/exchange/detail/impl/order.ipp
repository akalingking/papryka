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
 * @file        order.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 23, 2016 11:58 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
template <typename _T> Order::StateTable Order::state_table<_T>::value[] = {
    { Order::Initial, { Order::Submitted, Order::Canceled, Order::StateNone}},
    { Order::Submitted, { Order::Accepted, Order::Canceled, Order::StateNone}},
    { Order::Accepted, { Order::Filled, Order::PartiallyFilled, Order::Canceled, Order::StateNone}},
    { Order::PartiallyFilled, { Order::PartiallyFilled, Order::Filled, Canceled, Order::StateNone}},
    { Order::StateNone, { Order::StateNone}},
};

struct Order::Info
{
    typedef std::shared_ptr<Info> ptr_t;
    enum Error { ErrorNone=0, OrderCanceled = 1 };
    static const char* to_str(Error error) { return order_names_t::errors[error]; }
    
    real_t price;
    size_t quantity;
    real_t commission;
    datetime_t datetime;
    Error error;

    Info(real_t price, size_t quantity, real_t commission, const datetime_t& datetime) :
            price(price), quantity(quantity), commission(commission), datetime(datetime), error(ErrorNone)
    { log_trace("order::info created"); }
    ~Info() 
    { log_trace("order::info destroyed"); }
    Info() : error(ErrorNone), price(0), quantity(0), commission(0) {}
    Info(Error error) : error(error), price(0), quantity(0), commission(0) {}
};

struct Order::Event
{
    typedef std::shared_ptr<Event> ptr_t;
    enum Type { None = 0, Accepted = 1, Canceled = 2, PartiallyFilled = 3, Filled = 4 };
    static const char* to_str(Type type) { return order_names_t::types[type]; }

    datetime_t datetime;
    Order* order;
    Type type;
    Info::ptr_t info;

    Event(const datetime_t& datetime, Order* order, Type type, Info::ptr_t info) :
            datetime(datetime), order(order), type(type), info(info)
    { log_trace("Order::Event created {}", to_str(type)); }
    ~Event() 
    { log_trace("Order::Event destroyed {}", to_str(type)); }
};

Order::Order(Type type, Action action, const std::string& symbol, size_t quantity) :
        id(0), type(type), action(action), symbol(symbol), quantity(quantity), state(Initial),
        filled(0), avg_fill_price(0.0), is_all_or_none(false), is_good_till_canceled(false), commissions(0.0)
{
    log_trace("Order created");
}

size_t Order::get_valid_states(State state, std::set<State>& next_states) const
{
    int i = 0;
    next_states.clear();
    State current = state_table_.value[i].current;
    while (current != StateNone)
    {
        if (current == state)
        {
            std::vector<State> vec(std::begin(state_table_.value[i].valid_states), std::end(state_table_.value[i].valid_states));
            for (size_t i = 0; i < vec.size(); ++i)
                next_states.insert(vec[i]);
            break;
        }
        current = state_table_.value[++i].current;
    }
    return next_states.size();
}

bool Order::is_valid_state_(State current, State newState) const
{
    typedef std::set<State> states_t;
    states_t valid_states;
    get_valid_states(current, valid_states);
    states_t::const_iterator iter = std::find(valid_states.begin(), valid_states.end(), newState);
    return iter != valid_states.end();
}

void Order::switch_state(State newState)
{
    if (is_valid_state_(state, newState))
        state = newState;
    else
        log_error("Order::{} invalid new state={} in {}", to_str(newState), to_str(state));
}

void Order::set_submitted(uint32_t id, const datetime_t& datetime)
{
    if (this->id == 0)
    {
        this->id = id;
        this->submitted_date = datetime;
        log_debug("Order::{} id={} submitted={}", __func__, id, ::papryka::to_str(datetime));
    }
    else
    {
        assert(false);
    }
}

void Order::add_info(info_ptr_t& info)
{
    if (info->quantity > get_remaining())
    {
        std::stringstream strm;
//        strm << "Order" << "::" << __func__ << "Invalid fill size "
//            << get_remaining() << " and " << info->quantity << " filled" << std::endl;

        std::string str(strm.str());
        log_error(str.c_str());
        throw std::logic_error(str);
    }

    if (avg_fill_price == real_t(0.0))
        avg_fill_price = info->price;
    else
        avg_fill_price = ((avg_fill_price * filled)
        + (info->price * info->quantity)) / ((filled + info->quantity) * real_t(1.0));

    this->info = info;
    filled += precision::round(info->quantity);

    commissions += info->commission;

    if (get_remaining() == real_t(0.0))
    {
        switch_state(Filled);
    }
    else
    {
        assert(is_all_or_none == false);
        switch_state(PartiallyFilled);
    }
}
