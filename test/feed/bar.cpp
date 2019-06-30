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
 */
#include <gtest/gtest.h>
#include <papryka/detail/date.h>
#include <papryka/feed/frequency.h>
#include <papryka/feed/bar.h>

using namespace papryka;

TEST(Bar, basicConstruction)
{
    Bar bar(2,4,1,3);

    EXPECT_EQ(bar.open,2);
    EXPECT_EQ(bar.high,4);
    EXPECT_EQ(bar.low,1);
    EXPECT_EQ(bar.close,3);
    EXPECT_THROW(Bar(2,4,5,3), std::logic_error);
    EXPECT_THROW(Bar(2,1,3,3), std::logic_error);
}

TEST(Bar, invalidConstruction)
{
    EXPECT_THROW(Bar(2.0, 1.0, 1.0, 1.0, 1.0, 1.0), std::logic_error);
    EXPECT_THROW(Bar(1.0, 1.0, 1.0, 2.0, 1.0, 1.0), std::logic_error);
    EXPECT_THROW(Bar(1.0, 2.0, 1.5, 1.0, 1.0, 1.0), std::logic_error);
    EXPECT_THROW(Bar(2.0, 2.0, 1.5, 1.0, 1.0, 1.0), std::logic_error);
    EXPECT_THROW(Bar(1.0, 1.0, 1.5, 1.0, 1.0, 1.0), std::logic_error);
}

TEST(Bar, typicalPrice)
{
    Bar bar(2.0, 3.0, 1.0, 2.1, 5, 10);
    EXPECT_EQ(get_typical_price(bar), (3+1+2.1)/3.0);
}

TEST(Bar, adjustedClose)
{
    Bar bar(2, 3, 1, 2.1, 5, 10);
    EXPECT_EQ(get_adjusted(bar.close, bar.close, bar.closeadj), bar.closeadj);
}

TEST(Bar, noAdjClose)
{
    Bar bar(2, 3, 1, 2.1, 0, 10);
    EXPECT_THROW(get_adjusted(bar.open, bar.close, bar.closeadj), std::logic_error);
    EXPECT_THROW(get_adjusted(bar.high, bar.close, bar.closeadj), std::logic_error);
    EXPECT_THROW(get_adjusted(bar.low, bar.close, bar.closeadj), std::logic_error);
    EXPECT_THROW(get_adjusted(bar.close, bar.close, bar.closeadj), std::logic_error);
}
