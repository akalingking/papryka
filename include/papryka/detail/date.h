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
 * @file        date.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
#pragma once
#include <chrono>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <tuple>
#include <string.h>
#include "logger.h"
#include <ql/time/calendars/unitedstates.hpp>

namespace ql = QuantLib;

namespace std  {
namespace chrono {

typedef duration<int, ratio_multiply<chrono::hours::period, ratio<24> >::type> days;
typedef duration<int, ratio_multiply<chrono::hours::period, ratio<24*7> >::type> weeks;
typedef duration<int, ratio_multiply<chrono::hours::period, ratio<(365/12)*24> >::type> months;
typedef duration<int, ratio_multiply<chrono::days::period,  ratio<365> >::type> years;

}}

namespace papryka {

// Clock type
using Clock = std::chrono::high_resolution_clock;
// Duration types
using Months = std::chrono::months;
using Weeks = std::chrono::weeks;
using Days = std::chrono::days;
using Hours = std::chrono::hours;
using Minutes = std::chrono::minutes;
using Seconds = std::chrono::seconds;
using Milliseconds = std::chrono::milliseconds;
using Microseconds = std::chrono::microseconds;
using Nanoseconds = std::chrono::nanoseconds;
// Timepoint types
template<class Duration>
using Timepoint = std::chrono::time_point<Clock, Duration>;
typedef Timepoint<Days> date_t;
typedef Timepoint<Nanoseconds> datetime_t;
// Date formats
static const char* s_date_format = "%Y-%m-%d";
static const char* s_datetime_format = "%Y-%m-%d %H:%M:%S";
static const char* s_datetime_ms_format = "%Y-%m-%d %H:%M:%S.000000";
static const datetime_t nulldate = datetime_t::clock::from_time_t(0);
typedef std::vector<datetime_t> datetimes_t;

// operations
/**
 *
 * @param date
 * @param dateFormat
 * @return
 */
inline time_t to_time_t(const char* date, const char* dateFormat=s_datetime_ms_format);
/**
 *
 * @param date
 * @param dateFormat
 * @return
 */
inline datetime_t to_datetime(const char* date, const char* dateFormat=s_datetime_ms_format);
/**
 *
 * @param date
 * @param dateFormat
 * @return
 */
inline date_t to_date(const char* date, const char* dateFormat=s_date_format);
/**
 *
 * @param date
 * @return
 */
inline datetime_t to_datetime(const date_t& date);
/**
 *
 * @param datetime
 * @return
 */
inline date_t to_date(const datetime_t& datetime);
/**
 *
 * @param datetime
 * @param tz
 * @return
 */
inline datetime_t to_tz(const datetime_t& datetime, const char* tz);
/**
 *
 * @param date
 * @param dateFormat
 * @return
 */
inline const char* to_str(const date_t& date, const char* dateFormat=s_date_format);
/**
 * use inline to suppress unused warning compile message
 * it should really be static
 * @param date
 * @param dateFormat
 * @param tz
 * @return
 */
inline const char* to_str(const datetime_t& date, const char* dateFormat=s_datetime_format, const char* tz=nullptr);
/**
 *
 * @param datetime
 * @return
 */
inline int get_year(const datetime_t& datetime);
/**
 *
 * @param datetime
 * @return
 */
inline int get_month(const datetime_t& datetime);
/**
 *
 * @param datetime
 * @param tz
 * @return
 */
inline int get_day(const datetime_t& datetime, const char* tz="UTC");
/**
 *
 * @param datetime
 * @param tz
 * @return
 */
inline int get_hour(const datetime_t& datetime, const char* tz="UTC");
/**
 *
 * @param datetime
 * @return
 */
inline int get_minute(const datetime_t& datetime);
/**
 *
 * @param datetime
 * @return
 */
inline int get_second(const datetime_t& datetime);
/**
 *
 * @param datetime
 * @return
 */
inline unsigned long get_millisecond(const datetime_t& datetime);
/**
 *
 * @param datetime
 * @param tz
 * @return
 */
inline bool is_weekday(const datetime_t& datetime, const char* tz=nullptr);
/**
 * Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
 *                 m is in [1, 12]
 *                 d is in [1, last_day_of_month(y, m)]
 *                 y is "approximately" in
 *                   [numeric_limits<Int>::min()/366, numeric_limits<Int>::max()/366]
 *                 Exact range of validity is:
 *                 [civil_from_days(numeric_limits<Int>::min()),
 *                  civil_from_days(numeric_limits<Int>::max()-719468)]
 * @param y
 * @param m
 * @param d
 * @return number of days since civil 1970-01-01.  Negative values indicate
 * days prior to 1970-01-01.
 */
template <class Int>
constexpr Int days_from_civil(Int y, unsigned m, unsigned d) noexcept;
/**
 * Preconditions:  z is number of days since 1970-01-01 and is in the range:
 *                 [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
 * @param z
 * @return ear/month/day triple in civil calendar
 */
template <class Int>
constexpr std::tuple<Int, unsigned, unsigned>
civil_from_days(Int z) noexcept;
/**
 *
 * @param z
 * @return
 */
template <class Int>
constexpr unsigned
weekday_from_days(Int z) noexcept;
/**
 *
 * @param d
 * @return
 */
template <class To, class Rep, class Period>
To round_down(const std::chrono::duration<Rep, Period>& d);
/**
 *
 * @param tp
 * @return
 */
template <class Duration>
std::tm utc_tm(std::chrono::time_point<std::chrono::system_clock, Duration> tp);

typedef ql::Date qdatetime_t;
/**
 *
 * @param datetime
 * @return quantlib datetime format
 */
inline qdatetime_t to_qdate(const datetime_t datetime);

#include "./impl/date.ipp"
} // namespace papryka

