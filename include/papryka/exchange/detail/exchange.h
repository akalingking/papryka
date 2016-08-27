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
 * @date        July 23, 2016 5:55 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "../../detail/types.h"
#include "../../detail/precision.h"
#include "../../detail/event.h"
#include "../../exchange/order.h"
#include "../../exchange/commission.h"
#include "../../feed/feed.h"
#include "filltype.h"
#include <string>
#include <map>

namespace papryka {
namespace detail {

template <typename _T,  
        typename _Fill=FillType<_T>, 
        typename _Commission=NoCommission>
class Exchange
{
public:
    typedef _T value_t;
    typedef typename Traits<_T>::values_t values_t;
    typedef papryka::Order<_Fill> order_t;
    typedef Feed<_T> feed_t;
    typedef typename feed_t::ptr_t feed_ptr_t;
    typedef typename _Fill::info_t fill_info_t;
    typedef std::shared_ptr<order_t> order_ptr_t;
    typedef Order::event_ptr_t event_ptr_t;
    typedef Order::info_ptr_t info_ptr_t;
    typedef std::map<std::string, size_t> shares_t;
    typedef std::map<uint32_t, order_ptr_t> orders_t;

    feed_ptr_t feed;
    Event order_event;
    _Fill fillstrategy;
    _Commission commission;
    shares_t shares;
    bool use_adj_values;
    bool allow_fractions;
    bool allow_negative_cash;
    real_t cash;
    Precision precision_;
    
protected:
    orders_t orders_;
    Exchange(feed_ptr_t feed, real_t cash);
    void on_bars(const datetime_t& datetime, const values_t& values);
    bool register_order(order_ptr_t order);
    void unregister_order(uint32_t id);

private:
    void on_bars_imp(const datetime_t& datetime, const values_t& values, order_t* order);
    void process_order(const datetime_t& datetime, const value_t& value, order_t* order);
    bool pre_process_order(const datetime_t& datetime, const value_t& value, order_t* order);
    void post_process_order(const datetime_t& datetime, const value_t& value, order_t* order);
    bool commit_order_execution(const datetime_t& datetime, order_t* order, fill_info_t* fill);
};
    
#include "./impl/exchange.ipp"
}}