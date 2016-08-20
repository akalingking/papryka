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
 * @file        order.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 23, 2016 11:42 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/feed/bar.h>
#include <papryka/exchange/order.h>
#include <papryka/exchange/detail/filltype.h>
#include <papryka/detail/logger.h>
 
using namespace papryka;

TEST(Exchange, Order)
{
    typedef Traits<Bar>::values_t values_t;
    typedef detail::FillType<Bar> fill_t;
    typedef Order<fill_t> order_t;
    typedef typename fill_t::info_ptr_t fill_info_ptr_t;
    
    order_t order(order_t::Market, order_t::Buy, "GOOG", 1);
    fill_t fill;
    
    // create bars
    datetime_t datetime = datetime_t::clock::now();
    Bar bar(2, 4, 1, 3, 100);
    values_t values;
    values.insert(values_t::value_type("GOOG", bar));
    
    fill.on_bars(datetime, values);
    float vol = fill.get_volume_left("GOOG");
    log_debug("volume left {0:0.3f}", vol);
    EXPECT_EQ(vol, 100*.25);
    
    log_debug("Sample state string {}", order_t::to_str(order_t::Initial));
    
    fill_info_ptr_t ptr = order.process(fill, bar);
    log_debug("fill info price={0:0.3f} qty={1:0.3f}", ptr->price, ptr->quantity);
    EXPECT_EQ(ptr->price, 2);
    EXPECT_EQ(ptr->quantity, 1);
}

