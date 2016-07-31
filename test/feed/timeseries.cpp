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
 * @file        timeseries.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 9, 2016 4:52 PM
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/detail/date.h>
#include <papryka/feed/timeseries.h>
 
using namespace papryka;

TEST(Feed, timeseries)
{
    typedef Timeseries<float> timeseries_t;
    timeseries_t::row_t row;
    timeseries_t ts(Frequency::Day);
    row = timeseries_t::row_t(Clock::now(), 1.0);
    ts.push_back(row);
    EXPECT_EQ(std::get<1>(ts[0]), 1.0);

    row = timeseries_t::row_t(Clock::now(), 3.0);
    ts.push_back(row);
    const timeseries_t::row_t& row2 = ts[1];
    EXPECT_EQ(std::get<1>(row2), 3.0);
}
