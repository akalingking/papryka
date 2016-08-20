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
 * @file        exchange.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 23, 2016 11:42 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/exchange/exchange.h>
#include <papryka/feed/syntheticfeed.h>
#include <papryka/detail/dispatcher.h>
using namespace papryka;

TEST(Exchange, Exchange)
{
    datetime_t end = Clock::now();
    datetime_t start = end - std::chrono::days(100);
    std::shared_ptr<SyntheticFeed<Bar>> feed(new SyntheticFeed<Bar>(start, end, Frequency::Day));
    
    std::vector<std::string> symbols={"GOOG","MSFT"};
    feed->add_values_from_generator(symbols);

    Exchange exchange(feed, 1000);
    
    Dispatcher dispatcher;
    dispatcher.add_subject(feed);
    dispatcher.run();
    EXPECT_EQ(0,0);
}
