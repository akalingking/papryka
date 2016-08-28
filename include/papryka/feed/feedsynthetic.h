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
#include "../detail/typename.h"
#include "detail/gbm.h"
#include "feed.h"
#include "timeseries.h"
#include "bar.h"
#include <string>
#include <vector>
#include <ql/time/calendars/unitedstates.hpp>
#include <cassert>
#include <chrono>
#include <typeinfo>


namespace papryka {

// Generator implementation
struct GeneratorBase 
{
    static const constexpr size_t maxlen = 1024;
    datetime_t start;
    datetime_t end;
    Frequency frequency;
    
protected:
    inline GeneratorBase(const datetime_t& start, const datetime_t& end, Frequency frequency=Frequency::Day) :
            start(start), end(end), frequency(frequency) {}
};

//@{ Basic Random Generator 
struct Generator_ : GeneratorBase 
{
protected:
    typedef std::uniform_real_distribution<double> dist_t;
    std::default_random_engine rng;
    dist_t dist;
//    static const constexpr size_t maxlen=1024;
    inline Generator_(const datetime_t& start, const datetime_t& end, Frequency frequency);
};

template <typename _T> 
struct BasicGenerator;

template <>
struct BasicGenerator<real_t> : Generator_ {
    typedef typename Traits<real_t>::value_t value_t;
    typedef typename Traits<value_t>::row_t row_t;
    typedef typename Traits<value_t>::rows_t rows_t;
    typedef Timeseries<value_t> timeseries_t;
    inline BasicGenerator(const datetime_t& start, const datetime_t& end, Frequency frequency);
    inline size_t generate(rows_t& rows);
};

template <>
struct BasicGenerator<Bar> : Generator_ {
    typedef Bar value_t;
    typedef typename Traits<Bar>::row_t row_t;
    typedef typename Traits<Bar>::rows_t rows_t;
    typedef Timeseries<value_t> timeseries_t;
    inline BasicGenerator(const datetime_t& start, const datetime_t& end, Frequency frequency);
    inline size_t generate(rows_t& rows);
};
//@} Basic random generator

//@{ Geometric Brownian Motion Generator
template <typename _T>
struct GbmGenerator : GeneratorBase
{
    typedef _T value_t;
    typedef typename Traits<value_t>::row_t row_t;
    typedef typename Traits<value_t>::rows_t rows_t;
    typedef Timeseries<value_t> timeseries_t;
    _T start_value;
    real_t volatilty;
    real_t mean_return;
    GBM<value_t> impl;
    inline GbmGenerator(const datetime_t& start, const datetime_t& end, const value_t& startPrice, real_t volatilityPct, real_t meanRetPct, Frequency frequency=Frequency::Day);
    inline size_t generate(rows_t& rows);
};

template <>
struct GbmGenerator<Bar> : GeneratorBase
{
    typedef Bar value_t;
    typedef typename Traits<value_t>::row_t row_t;
    typedef typename Traits<value_t>::rows_t rows_t;
    typedef Timeseries<value_t> timeseries_t;
    typedef GBM<real_t> gbm_t; 
    gbm_t open;
    gbm_t high;
    gbm_t low;
    gbm_t  close; 
    gbm_t volume;
    inline GbmGenerator(const datetime_t& start, const datetime_t& end, const value_t& startPrice, real_t volatilityPct, real_t meanRetPct, Frequency frequency);
    inline size_t generate(rows_t& rows);
};
//@} Geometric Brownian Motion Generator

/**
 *@brief Synthetic feed uses GBM as default generator
 */
template<typename _T=real_t, typename _Generator=GbmGenerator<_T> >
class FeedSynthetic : public FeedMemFilter<_T> 
{
public:
    struct Data {
        std::string symbol;
        _T start_value;
        real_t annual_volatility;
        real_t annual_mean_return;
    };
    typedef FeedMemFilter<_T> base_t;
    typedef typename Traits<_T>::rows_t rows_t;
    typedef std::vector<Data> data_t;
    FeedSynthetic(datetime_t start, const datetime_t& end, Frequency frequency=Frequency::Day, size_t maxlen=1024);
    void add_values_from_generator(const data_t& data);

private:
    datetime_t start_;
    datetime_t end_;
};
    
#include "impl/feedsynthetic.ipp"
}
