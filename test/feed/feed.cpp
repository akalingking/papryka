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
 * @file        feed.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 16, 2016 5:13 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/detail/date.h>
#include <papryka/detail/dispatcher.h>
#include <papryka/feed/feedsynthetic.h>

using namespace papryka;

TEST(Feed, Float)
{
    datetime_t end = Clock::now();
    datetime_t start = end - std::chrono::days(100);
    
    typedef FeedSynthetic<real_t> feed_t;
    std::shared_ptr<feed_t> feed(new feed_t(start, end, Frequency::Day));
    feed_t::data_t data = {
        {"GOOG", 770, 0.2, 0.1}, 
        {"MSFT", 60, 0.2, 0.1},
        {"QQQ", 117, 0.2, 0.1}};
    feed->add_values_from_generator(data);
    
    Dispatcher dispatcher;
    dispatcher.add_subject(feed);
    dispatcher.run();
    
    EXPECT_EQ(0,0);
}

TEST(Feed, Bar)
{ 
    datetime_t end = Clock::now();
    datetime_t start = end - std::chrono::days(100);
    
    typedef FeedSynthetic<Bar> feed_t;
    std::shared_ptr<feed_t> feed(new feed_t(start, end, Frequency::Day));
    
    Bar goog_start_value(771,773,770,772,1350000);
    Bar msft_start_value(59,61,58,60,15000000);
    Bar qqq_start_value(116,118,115,117,17360000);
    
    feed_t::data_t data = {
        // symbol, spot, volatility, exp profit
        {"GOOG", goog_start_value, 0.2, 0.1}, 
        {"MSFT", msft_start_value, 0.2, 0.1},
        {"QQQ", qqq_start_value, 0.2, 0.1}};
    
    feed->add_values_from_generator(data);
    
    Dispatcher dispatcher;
    dispatcher.add_subject(feed);
    dispatcher.run();
    
    EXPECT_EQ(0,0);
}
