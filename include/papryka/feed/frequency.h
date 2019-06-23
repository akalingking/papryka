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
 * @file        frequency.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
#pragma once
#include  "../detail/date.h"

namespace papryka {

enum Frequency { Day=0, Hour, Minute, Second, Millisecond, Microsecond, Tick };

static const char* frequency_str[] { "Day", "Hour", "Minute", "Second", "Millisecond", "Microsecond", "Tick" };

static const char* to_str(Frequency e) { return frequency_str[e]; }

static datetime_t get_next_timepoint(const datetime_t& datetime, Frequency frequency)
{
    assert (datetime != nulldate);

    switch(frequency) {
        case Frequency::Day:
            return datetime + std::chrono::days(1);
        case Frequency::Hour:
            return datetime + std::chrono::hours(1);
        case Frequency::Minute:
            return datetime + std::chrono::minutes(1);
        case Frequency::Second:
            return datetime + std::chrono::seconds(1);
        case Frequency::Millisecond:
            return datetime + std::chrono::milliseconds(1);
        case Frequency::Microsecond:
            return datetime + std::chrono::microseconds(1);
        default:
            assert (false);
    }
}

// will not work unless _Rep and _Duration is supplied in call (e.g. get_duration<type, type>(frequency))
template <typename _Rep, typename _Duration>
static auto get_duration(Frequency frequency)->decltype(std::chrono::duration<_Rep, _Duration>())
{
    switch(frequency)
    {
        case Frequency::Day:
            return std::chrono::days(1);
        case Frequency::Hour:
            return std::chrono::hours(1);
        case Frequency::Minute:
            return std::chrono::minutes(1);
        case Frequency::Second:
            return std::chrono::seconds(1);
        case Frequency::Millisecond:
            return std::chrono::milliseconds(1);
        case Frequency::Microsecond:
            return std::chrono::microseconds(1);
        default:
            assert (false);
    }
}

template<typename _Rep, typename _Period>
static size_t get_bars_per_day(const std::chrono::duration<_Rep,_Period>& duration)
{
    return (size_t)std::chrono::days(1)/duration;
}

static size_t get_bars_per_day(Frequency frequency)
{
    switch(frequency) {
        case Frequency::Day:
            return std::chrono::days(1)/std::chrono::days(1);
        case Frequency::Hour:
            return std::chrono::days(1)/std::chrono::hours(1);
        case Frequency::Minute:
            return std::chrono::days(1)/std::chrono::minutes(1);
        case Frequency::Second:
            return std::chrono::days(1)/std::chrono::seconds(1);
        case Frequency::Millisecond:
            return std::chrono::days(1)/std::chrono::milliseconds(1);
        case Frequency::Microsecond:
            return std::chrono::days(1)/std::chrono::microseconds(1);
        default:
            assert (false);
    }
}

}
