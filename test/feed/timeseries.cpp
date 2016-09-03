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
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/papryka.h>
#include <papryka/feed/timeseries.h>
 
using namespace papryka;

TEST(Feed, timeseries)
{
    typedef Timeseries<real_t> timeseries_t;
    timeseries_t::row_t row;
    timeseries_t ts(Frequency::Day);
    row = timeseries_t::row_t(Clock::now(), real_t(1.0));
    ts.push_back(row);

    row = timeseries_t::row_t(Clock::now(), real_t(3.0));
    ts.push_back(row);
    const timeseries_t::row_t& row2 = ts[1];
    EXPECT_EQ(std::get<1>(row2), real_t(3.0));
}
