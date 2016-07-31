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
 * @file        synthetic.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 16, 2016 1:01 AM
 * @copyright   2017 www.sequenceresearch.com
 */
#define SPDLOG_DEBUG_ON
//#define SPDLOG_TRACE_ON

#include <papryka/papryka.h>
#include <papryka/feed/syntheticfeed.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

using namespace papryka;

int main(int argc, char** argv)
{
    datetime_t start = to_datetime("2010-01-01");
    datetime_t end 	 = to_datetime("2010-12-31");
    
    std::shared_ptr<SyntheticFeed<float>> ptr(
            new SyntheticFeed<float>(start, end, Frequency::Day));
    
    std::vector<std::string> symbols={"QQQ", "AAA", "BBB"};

    ptr->add_values_from_generator(symbols);
    
    Dispatcher dispatcher;
    dispatcher.add_subject(ptr);
    dispatcher.run();
    
    std::cin.get();
    return 0;
}