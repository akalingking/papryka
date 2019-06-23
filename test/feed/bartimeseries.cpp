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
 * @file        bartimeseries.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/papryka.h>
#include <papryka/feed/bartimeseries.h>

using namespace papryka;

TEST(Bartimeseries, custom_bar)
{
	typedef Timeseries<Bar> timeseries_t;
	timeseries_t::row_t row;
	timeseries_t ts(Frequency::Day);
	Bar bar1(2.0, 3.0, 1.0, 2.1, 5, 10);
	row = timeseries_t::row_t(Clock::now(), bar1);
	ts.push_back(row);
	EXPECT_EQ(ts.size(), 1);
	EXPECT_EQ(ts.column_size(), 5);

	Bar bar2(2.0, 3.0, 1.0, 2.1, 5, 10);
	row = timeseries_t::row_t(Clock::now(), bar2);
	ts.push_back(row);
	EXPECT_EQ(ts.size(), 2);
	EXPECT_EQ(ts.column_size(), 5);
}
