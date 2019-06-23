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
 * @file        date.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/papryka.h>

using namespace papryka;

TEST(detail, date)
{
    datetime_t date = Clock::now();
    log_debug("current date='{}' ", to_str(date));
}

TEST(detail, dateString)
{
    std::string strdate = "2000-11-03";
    date_t date = to_date(strdate.c_str(), s_date_format);
    log_debug("exp={} date={}", strdate, to_str(date));
    EXPECT_TRUE(!string::compare(strdate.c_str(), to_str(date)));
}

TEST(detail, datetimeString)
{
    std::string strdatetime = "2000-11-03 10:20:30";
    datetime_t datetime = to_datetime(strdatetime.c_str(), s_datetime_format);
    log_debug("exp='{}' datetime='{}'", strdatetime, to_str(datetime, s_datetime_format));
    EXPECT_TRUE(!string::compare(strdatetime.c_str(), to_str(datetime, s_datetime_format)));
}

TEST(detail, datetimeMsString)
{
    std::string strdatetime = "2000-11-03 10:20:30.500";
    datetime_t datetime = to_datetime(strdatetime.c_str(), s_datetime_ms_format);
    log_debug("exp='{}' datetime='{}'", strdatetime, to_str(datetime, s_datetime_ms_format));
    EXPECT_TRUE(!string::compare(strdatetime.c_str(), to_str(datetime, s_datetime_ms_format)));
}
