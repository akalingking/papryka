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
 * @date        July 2, 2016 5:46 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
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

TEST(Bar, testInvalidConstruction)
{
    EXPECT_THROW(Bar(2.0, 1.0, 1.0, 1.0, 1.0, 1.0), std::logic_error);
    EXPECT_THROW(Bar(1.0, 1.0, 1.0, 2.0, 1.0, 1.0), std::logic_error);
    EXPECT_THROW(Bar(1.0, 2.0, 1.5, 1.0, 1.0, 1.0), std::logic_error);
    EXPECT_THROW(Bar(2.0, 2.0, 1.5, 1.0, 1.0, 1.0), std::logic_error);
    EXPECT_THROW(Bar(1.0, 1.0, 1.5, 1.0, 1.0, 1.0), std::logic_error);
}
