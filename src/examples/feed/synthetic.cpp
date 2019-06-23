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
 * @copyright   (c) <www.sequenceresearch.com>
 */
#include <papryka/papryka.h>
#include <papryka/feed/feedsynthetic.h>
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>

using namespace papryka;

int main(int argc, char** argv)
{
    typedef FeedSynthetic<real_t> feed_t;

    datetime_t start = to_datetime("2010-01-01");
    datetime_t end 	 = to_datetime("2010-12-31");

    std::shared_ptr<feed_t> ptr(new feed_t(start, end, Frequency::Day));

    std::vector<feed_t::Data> data = {
        {"QQQ", 115, 0.2, 0.1},
        {"MSFT", 60, 0.2, 0.1},
        {"GOOG", 771, 0.2, 0.1}};

    ptr->add_values_from_generator(data);

    Dispatcher dispatcher;
    dispatcher.add_subject(ptr);
    dispatcher.run();
    return 0;
}
