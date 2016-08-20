
template <typename _Fill>
Order<_Fill>::Order(base_t::Type type, base_t::Action action, const std::string& symbol, int quantity, bool isFillOnClose, float limitPrice, float stopPrice) :
        base_t(type, action, symbol, quantity), is_fill_on_close(isFillOnClose), limit_price(limitPrice), stop_price(stopPrice)
{

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