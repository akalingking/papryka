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
 * @file        exchange.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 23, 2016 11:46 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "./detail/exchange.h"
#include "./detail/filltype.h"
#include "commission.h"
#include "order.h"

namespace papryka {
 
template <typename _T=Bar>
class Exchange : public detail::Exchange<_T>  
{
public:
    typedef std::shared_ptr<Exchange> ptr_t;
    typedef Bar value_t;
    typedef typename Traits<value_t>::values_t values_t; 
    typedef typename detail::Exchange<value_t> base_t;
    typedef typename base_t::feed_t feed_t;
    typedef typename base_t::feed_ptr_t feed_ptr_t;
    typedef typename base_t::order_t order_t;
    typedef typename base_t::order_ptr_t order_ptr_t;

    Exchange(feed_ptr_t feed, real_t cash);
    virtual ~Exchange();
    bool submit_order(order_ptr_t order);
    bool cancel_order(uint32_t id);
    real_t get_equity();
    order_ptr_t create_order(order_t::Type type, order_t::Action action, const std::string& symbol, size_t quantity, 
            bool isFillOnClose=false, real_t stopPrice=real_t(0), real_t limitPrice=real_t(0));

private:
    template <typename _U> struct order_id { static uint32_t value; };
    typedef order_id<void> order_id_t;
    order_id_t order_id_;    
};
    
#include "impl/exchange.ipp"
}
