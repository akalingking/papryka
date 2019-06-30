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
 * @file        timeseries.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 */
#pragma once
#include "../detail/types.h"
#include "../detail/date.h"
#include "../detail/event.h"
#include "../detail/traits.h"
#include "frequency.h"
#include <memory>
#include <deque>
#include <exception>
#include <type_traits>
#include "bar.h"

namespace papryka {

static const constexpr size_t s_timeseries_max_len = 1024*20;

template<typename _T=real_t, typename _Alloc=std::allocator<_T> >
class Timeseries
{
public:
    typedef typename Traits<_T>::value_t value_t;
    typedef typename Traits<_T>::row_t row_t;
    typedef typename Traits<_T>::rows_t rows_t;
    typedef typename rows_t::iterator iterator_t;
    typedef typename rows_t::const_iterator const_iterator_t;

    Frequency frequency;
    Event event_new_value;
    size_t maxlen;

    Timeseries(Frequency frequency=Frequency::Day, size_t maxlen=s_timeseries_max_len, Event* eventSource=nullptr);
    ~Timeseries();
    size_t size() const;
    void clear();
    bool empty() const;
    iterator_t& begin();
    const_iterator_t& begin() const;
    iterator_t& end();
    const_iterator_t& end() const;
    void push_back(const row_t& row);
    row_t pop_front();
    const row_t& operator[](size_t pos) const;
    row_t& operator[](size_t pos);
    size_t column_size() const { return column_size_<_T>(); }

private:
    // overloads
    template <typename _U, typename _K=std::enable_if_t<(std::is_arithmetic<_U>::value) || (std::is_same<_U, Bar>::value), real_t>>
	size_t column_size_() const { return (std::is_same<_U, Bar>::value) ? 5 : 1; }

    template <typename _U, typename _K=std::enable_if_t<(!std::is_arithmetic<_U>::value) && (!std::is_same<_U, Bar>::value), _U>>
    size_t column_size_(_K* k=0) const { return std::tuple_size<_U>::value; }

    void on_new_value_(const row_t& row);
    rows_t rows_;
};

typedef Timeseries<> Timeseries_t;

#include "impl/timeseries.ipp"
}





