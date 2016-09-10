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
 * @file        fillstrategy.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 23, 2016 6:12 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "../../detail/traits.h"
#include "../../feed/bar.h"
#include "../../feed/frequency.h"
#include "../../detail/precision.h"
#include "../../detail/date.h"
#include "../slippage.h"
#include "order.h"
#include <map>
#include <string>
#include <cassert>
#include <type_traits>

namespace papryka {
namespace detail {

struct FillTrigger 
{
    typedef Traits<Bar>::value_t value_t;
    inline real_t get_stop_price_trigger(Order::Action action, real_t stopPrice, bool useAdjValues, const value_t& value);
    inline real_t get_limit_price_trigger(Order::Action action, real_t limitPrice, bool useAdjValues, const value_t& value);
};

struct FillInfo 
{
    FillInfo(real_t price, size_t quantity) : price(price), quantity(quantity) {}
    real_t price;
    size_t quantity;
};

/**
 * @brief Used by Order types to fill order executions.
 * Order type is a function template argument since this object is a
 * template parameter to Order types.  
 * @param _T is the bar type
 */
template <typename _T>
class FillType {};

/**
 * @brief FillType specialization using Bar type as internal data source
 */
template<>
class FillType<Bar>
{
public:
    typedef Bar value_t;
    typedef Traits<value_t>::values_t values_t;
    typedef NoSlippage slippage_t;
    typedef FillInfo info_t;
    typedef std::shared_ptr<info_t> info_ptr_t;
    typedef std::map<std::string, real_t> volumes_t;
    Frequency frequency;

    inline FillType(Frequency frequency = Frequency::Day, real_t volumeLimit = real_t(0.25));
    inline void on_bars(const datetime_t& date, const values_t& values);
    inline real_t get_volume_left(const std::string& symbol);

    template <typename _Order>
    void on_order_filled(_Order& order);

    template <typename _Order>
    info_ptr_t fill(_Order& order, const value_t& value, market_order_tag&&);

    template <typename _Order>
    info_ptr_t fill(_Order& order, const value_t& bar, limit_order_tag&&);

    template <typename _Order>
    info_ptr_t fill(_Order& order, const value_t& bar, stop_order_tag&&);

    template <typename _Order>
    info_ptr_t fill(_Order& order, const value_t& bar, stop_limit_order_tag&&);

private:    
    slippage_t slippage_;
    real_t volume_limit_;
    volumes_t volume_left_;
    volumes_t volume_used_;
    
    template <typename _Order>
    real_t calculate_fill_size_(_Order& order, const value_t& bar);
};

#include "impl/filltype.ipp"
}}