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
template <typename _D, typename _T>
Strategy<_D,_T>::Position::StateMachine::StateMachine(Position& position) :
        position(position), state(StateIdle)
{
    log_trace("Position::StateMachine::{}", __func__);
}

template <typename _D, typename _T>
Strategy<_D,_T>::Position::StateMachine::~StateMachine()
{
    log_trace("Position::StateMachine::{}", __func__);
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::StateMachine::switch_state(State newState)
{
    if (state != newState)
    {
        state = newState;
        enter();
    }
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::StateMachine::enter()
{
    log_trace("Position::StateMachine::{} state={}", __func__, to_str(state));

    switch (state)
    {
        case StateIdle:
            //noop
            break;
        case StateOpen:
            {
                order_t* entry_order = position.entry_order.get();
                if (entry_order)
                {
                    position.entry_datetime = entry_order->info->datetime;
                    log_debug("Position::StateMachine::{} entry datetime={}", __func__, papryka::to_str(position.entry_datetime));
                }
            }
            break;
        case StateClose:
            {
                order_t* exit_order = position.exit_order.get();
                if (exit_order && exit_order->is_filled())
                {
                    // error datetime is wrong here
                    position.exit_datetime = exit_order->info->datetime;
                    log_debug("Position::StateMachine::{} exit datetime={}", __func__, papryka::to_str(position.exit_datetime));
                }
            }
            break;
        default:
            assert (false);
    }
}

template <typename _D, typename _T>
bool Strategy<_D,_T>::Position::StateMachine::is_open() const
{
    bool ret = (state == StateOpen);
    log_debug("StateMachine::{} state={} {}", __func__, to_str(state), ret?"true":"false");
    return (state == StateOpen);
}

template <typename _D, typename _T>
bool Strategy<_D,_T>::Position::StateMachine::can_submit_order(order_t& order)
{
    bool ret = false;
    switch (state)
    {
        case StateIdle:
            ret = (position.entry_order.get() && position.entry_order->is_active());
            break;
        case StateOpen:
            ret = true;
            break;
        case StateClose:
            break;
        default:
            break;
    }
    return ret;
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::StateMachine::on_order_event(order_event_t& event)
{
    log_trace("Position::StateMachine::{} event={} state={} datetime={} entry", __func__,
            order_t::Event::to_str(event.type), to_str(state), papryka::to_str(event.datetime));

    Strategy& strategy = position.strategy;
    order_t* order = (order_t*)event.order;
    order_ptr_t& entry_order = position.entry_order;
    order_ptr_t& exit_order = position.exit_order;
    switch (state)
    {
        case StateIdle:
            if (event.type == order_t::Event::Filled || event.type == order_t::Event::PartiallyFilled)
            {
                log_debug("StateMachine::{} {} entry order fill size={} was filled", __func__,
                        papryka::to_str(event.datetime), entry_order->filled);
                switch_state(StateOpen);
                strategy.on_enter(&position);
            }
            else if (event.type == order_t::Event::Canceled)
            {
                log_debug("StateMachine::{} {} entry order fill size={} was canceled",__func__,
                        papryka::to_str(event.datetime), entry_order->filled);
                assert (entry_order->filled == 0);

                switch_state(StateClose);
                strategy.on_enter_canceled(&position);
                exit(0,0,false);
            }
            else
            {
                log_warn("StateMachine::{} {} position id={} event={} ignored in state={}!", __func__,
                        papryka::to_str(event.datetime), position.id, order_t::Event::to_str(event.type), to_str(state));
            }
            break;
        case StateOpen:
            if (order)
            {
                if (exit_order->id == order->id)
                {
                    if (event.type == order_t::Event::Filled)
                    {
                        assert (exit_order->info.get());
                        assert (order->info.get());

                        if (position.shares_ == 0)
                        {
                            switch_state(StateClose);

                            strategy.on_exit(&position);

                            // deletes position, parameters does not matter
                            //can be called on exit
                            exit(0, 0, false);
                        }
                    }
                    else if (event.type == order_t::Event::Canceled)
                    {
                        assert(position.shares_ != 0);
                        strategy.on_exit_canceled(&position);
                    }
                    else
                    {
                        log_warn("Position_::{} position id={} event={} ignored", __func__,
                                position.id, order_t::Event::to_str(event.type));
                    }
                }
                else
                    log_error("Position_::{} {} position id={} exit order id={}, event order id={} do not match!",
                            __func__, papryka::to_str(event.datetime), position.id, exit_order->id, order->id);
            }
            else
            {
                log_error("Position_::{} {} event order is empty in state={}!", __func__,
                        papryka::to_str(event.datetime), to_str(state));
            }
            break;
        case StateClose:
            log_error("Position_::{} position is already closed!", __func__);
            assert (false && "position is closed");
            break;
        default:
            assert (false);
    }

    log_trace("Position::StateMachine::{} event={} state={} exit", __func__,
            order_t::Event::to_str(event.type), to_str(state));
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::StateMachine::exit(real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled)
{
    log_trace("Position::StateMachine::{} state={}", __func__, to_str(state));

    Strategy& strategy = position.strategy;
    order_t* entry_order = position.entry_order.get();
    switch (state)
    {
        case StateIdle:
            assert (position.shares_ == 0);
            if (entry_order && entry_order->is_active())
                strategy.exchange->cancel_order(entry_order->id);
            break;
        case StateOpen:
            {
                assert (entry_order);
                order_t* exit_order = position.exit_order.get();

                if (exit_order && exit_order->is_active())
                {
                    log_error("Position::StateMachine::{} exit order is active", __func__);
                    throw std::logic_error("exit order is active");
                }

                if (entry_order->is_active())
                    strategy.exchange->cancel_order(entry_order->id);
                else
                    position.submit_exit_order(stopPrice, limitPrice, isGoodTillCanceled);
            }
            break;
        case StateClose:
            strategy.unregister_position(position.id);
            break;
        default:
            assert (false);
    }
}
