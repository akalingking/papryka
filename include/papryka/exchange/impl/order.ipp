
template <typename _Fill>
Order<_Fill>::Order(base_t::Type type, base_t::Action action, const std::string& symbol, size_t quantity, bool isFillOnClose, real_t stopPrice, real_t limitPrice) :
        base_t(type, action, symbol, quantity), is_fill_on_close(isFillOnClose), stop_price(stopPrice), limit_price(limitPrice), is_stop_hit(false), is_limit_hit(false)
{
    log_trace("Order<_Fill>::{}", __func__);
}

template <typename _Fill>
Order<_Fill>::~Order()
{
    log_trace("Order<_Fill>::{}", __func__);
}

template <typename _Fill>
typename Order<_Fill>::fill_info_ptr_t Order<_Fill>::process(fill_t& fill, const value_t& value)
{
    log_debug("Order<_Fill>::{} {}", __func__, value);
    
    switch (base_t::type)
    {
        case base_t::Market:
            return fill.fill(*this, value, detail::order_traits<base_t::Market>::type());
        case base_t::Limit:
            return fill.fill(*this, value, detail::order_traits<base_t::Limit>::type());
        case base_t::Stop:
            return fill.fill(*this, value, detail::order_traits<base_t::Stop>::type());
        case base_t::StopLimit:
            return fill.fill(*this, value, detail::order_traits<base_t::StopLimit>::type());
        default:
            log_error("Order<_Fill>::{} fill type not implemented", __func__);
            assert (false);
            return 0;
    }
}