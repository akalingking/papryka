
template <typename _Fill>
Order<_Fill>::Order(base_t::Type type, base_t::Action action, const std::string& symbol, size_t quantity, bool isFillOnClose, real_t limitPrice, real_t stopPrice) :
        base_t(type, action, symbol, quantity), is_fill_on_close(isFillOnClose), limit_price(limitPrice), stop_price(stopPrice)
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
    switch (base_t::type)
    {
        case base_t::Market:
            typename detail::order_traits<base_t::Market>::type tag;
            return fill.fill(*this, value, tag);
            //...
        default:
            return 0;
    }
}