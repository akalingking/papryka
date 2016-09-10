
template <typename _D, typename _T>
Strategy<_D,_T>::Position::StateMachine::StateMachine(Position& position) : 
        position(position), state(StateIdle)
{}

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
    switch (state) 
    {
        case StateIdle:
            break;
        case StateOpen:
            if (position.entry_order)
                position.entry_datetime = position.entry_order->info->datetime;
        case StateClose:
            if (position.exit_order && position.exit_order->is_filled())
                position.exit_datetime = position.exit_order->info->datetime;
        default:
            assert (false);
    }
}

template <typename _D, typename _T>
bool Strategy<_D,_T>::Position::StateMachine::is_open() const
{
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
        case StateOpen:
            ret = true;
        case StateClose:
        default:
            break;
    }
    return ret;
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::StateMachine::on_order_event(order_event_t& event)
{
    Strategy& strategy = position.strategy;
    order_t* order = (order_t*)event.order;
    order_ptr_t& entry_order = position.entry_order;
    order_ptr_t& exit_order = position.exit_order;
    switch (state) 
    {
        case StateIdle:
            if (event.type == order_t::Event::Filled || event.type == order_t::Event::PartiallyFilled)
            {
                log_debug("StateMachine::{0:} {1:} entry order fill size={2:0.3f} was filled", __func__,  papryka::to_str(event.datetime), entry_order->filled);
                switch_state(StateOpen);    
            }
            else if (event.type == order_t::Event::Canceled)
            {
                log_debug("StateMachine::{0:} {1:} entry order fill size={2:0.3f} was canceled",__func__, papryka::to_str(event.datetime), entry_order->filled);
                assert (entry_order->filled == 0);
                // determine which comes first
                switch_state(StateClose);
                strategy.on_enter_canceled(&position);
                exit(0,0,false);
            }
            else
            {
                log_warn("StateMachine::{} {} position id={} event={} ignored!", __func__, papryka::to_str(event.datetime), position.id, order_t::Event::to_str(event.type));
            }
            break;
        case StateOpen:
            if (order)
            {
                if (exit_order->id == order->id)
                {
                    if (event.type == order_t::Event::Filled)
                    {
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
                        log_warn("Position_::{} position id={} event={} ignored", __func__, position.id, order_t::Event::to_str(event.type));
                    }
                }
                else
                    log_error("Position_::{} {} position id={} exit order id={}, event order id={} do not match!", 
                            __func__, papryka::to_str(event.datetime), position.id, exit_order->id, order->id);
            }
            else
            {
                log_error("Position_::{} {} event order is empty!", __func__,  papryka::to_str(event.datetime));
            }
            break;
        case StateClose:
            assert (false && "position is closed");
            break;
        default:
            assert (false);
    }
}

template <typename _D, typename _T>
void Strategy<_D,_T>::Position::StateMachine::exit(real_t stopPrice, real_t limitPrice, bool isGoodTillCanceled)
{
    Strategy& strategy = position.strategy;
    order_ptr_t& entry_order = position.entry_order;
    order_ptr_t& exit_order = position.exit_order;
    switch (state) 
    {
        case StateIdle:
            assert (position.shares_ == 0);
            assert (entry_order->is_active());
            strategy.exchange->cancel_order(entry_order->id);
            break;
        case StateOpen:
            if (!exit_order->is_active()) 
            {
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
