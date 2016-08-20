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
 * @file        syntheticfeed.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 16, 2016 6:16 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "../detail/date.h"
#include "../detail/traits.h"
#include "feed.h"
#include "timeseries.h"
#include "bar.h"
#include <string>
#include <vector>
#include <ql/time/calendars/unitedstates.hpp>
#include <cassert>
#include <chrono>

namespace papryka {

// Generator implementation
struct FeedGenerator_ {
protected:
    typedef std::uniform_real_distribution<double> dist_t;
    datetime_t start;
    datetime_t end;
    Frequency frequency;
    std::default_random_engine rng;
    dist_t dist;
    static const constexpr size_t maxlen=1024;
    FeedGenerator_(const datetime_t& start, const datetime_t& end, Frequency frequency);
};

// parent template class
template <typename _T> struct FeedGenerator;

template <>
struct FeedGenerator<float> : FeedGenerator_ {
    typedef typename Traits<float>::value_t value_t;
    typedef typename Traits<value_t>::row_t row_t;
    typedef typename Traits<value_t>::rows_t rows_t;
    typedef Timeseries<value_t> timeseries_t;
    inline FeedGenerator(const datetime_t& start, const datetime_t& end, Frequency frequency);
    inline size_t generate(rows_t& rows);
};

template <>
struct FeedGenerator<Bar> : FeedGenerator_ {
    typedef Bar value_t;
    typedef typename Traits<Bar>::row_t row_t;
    typedef typename Traits<Bar>::rows_t rows_t;
    typedef Timeseries<value_t> timeseries_t;
    inline FeedGenerator(const datetime_t& start, const datetime_t& end, Frequency frequency);
    inline size_t generate(rows_t& rows);
};

#if 0
/**
 * used only for testing the template version
 */
struct GenericGenerator {
    typedef std::uniform_real_distribution<double> dist_t;
    typedef float value_t;
    typedef Timeseries<value_t> timeseries_t;
    typedef typename timeseries_t::row_t row_t;
    typedef typename timeseries_t::rows_t rows_t;

    datetime_t start;
    datetime_t end;
    Frequency frequency;
    std::default_random_engine rng;
    dist_t dist;
    static const constexpr size_t max_len=1024;

    GenericGenerator(const datetime_t& start, const datetime_t& end, Frequency frequency);
    size_t generate(rows_t& rows);
};
#endif

template<typename _T=float, typename _Generator=FeedGenerator<_T> >
class SyntheticFeed : public FilterMemFeed<_T> {
public:
    typedef FilterMemFeed<_T> base_t;
    typedef typename Traits<_T>::rows_t rows_t;
    typedef std::vector<std::string> names_t;
    SyntheticFeed(datetime_t start, const datetime_t& end, Frequency frequency);
    void add_values_from_generator(const std::vector<std::string>& instruments);

private:
    datetime_t start_;
    datetime_t end_;
    _Generator generator_;
};
    
#include "impl/syntheticfeed.ipp"
}
