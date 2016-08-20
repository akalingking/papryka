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
 * @copyright   2017 www.sequenceresearch.com
 */
#pragma once
#include "../feed/bar.h"
#include "../detail/date.h"
#include "../detail/precision.h"
#include "detail/order.h"
#include "detail/filltype.h"
#include <memory>
#include <string>
#include <cassert>

namespace papryka {

template <typename _Fill = detail::FillType<Bar>>
class Order : public detail::Order
{
public:
    typedef detail::Order base_t;
    typedef _Fill fill_t;
    typedef typename fill_t::value_t value_t;
    typedef typename fill_t::info_ptr_t fill_info_ptr_t;
    typedef typename base_t::Event event_t;
    typedef typename base_t::event_ptr_t event_ptr_t;

    using base_t::Type;
    using base_t::State;
    using base_t::Action;
    using base_t::Info;
    using base_t::Event;
            
    bool is_fill_on_close;
    bool is_limit_hit;
    bool is_stop_hit;
    float stop_price;
    float limit_price;
    
    explicit Order(base_t::Type type, base_t::Action action, const std::string& symbol, int quantity, bool isFillOnClose=false, float limitPrice=0.0, float stopPrice=0.0);
    fill_info_ptr_t process(fill_t& fill, const value_t& value);
};

#include "impl/order.ipp"
}
