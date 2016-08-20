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
#include <papryka/feed/syntheticfeed.h>

using namespace papryka;

TEST(Feed, FeedFloat)
{
    datetime_t end = Clock::now();
    datetime_t start = end - std::chrono::days(100);
    
    std::shared_ptr<SyntheticFeed<float>> feed(new SyntheticFeed<float>(start, end, Frequency::Day));
    
    std::vector<std::string> symbols={"GOOG","MSFT"};
    feed->add_values_from_generator(symbols);
    
    Dispatcher dispatcher;
    dispatcher.add_subject(feed);
    dispatcher.run();
    
    EXPECT_EQ(0,0);
}

TEST(Feed, FeedBar)
{
    datetime_t end = Clock::now();
    datetime_t start = end - std::chrono::days(100);
    
    std::shared_ptr<SyntheticFeed<Bar>> feed(new SyntheticFeed<Bar>(start, end, Frequency::Day));
    
    std::vector<std::string> symbols={"GOOG","MSFT"};
    feed->add_values_from_generator(symbols);
    
    Dispatcher dispatcher;
    dispatcher.add_subject(feed);
    dispatcher.run();
    
    EXPECT_EQ(0,0);
}



