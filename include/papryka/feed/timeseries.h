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
 * @date        July 2, 2016 10:42 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
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

namespace papryka {
    
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

    Timeseries(size_t max=1024, Frequency frequency=Frequency::Day, Event* eventSource=nullptr);
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

private:
    void on_new_value_(const row_t& row);
    rows_t rows_;
};
    
#include "impl/timeseries.ipp"
}





