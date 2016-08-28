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
 * @file        filter.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 16, 2016 5:19 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "../../detail/types.h"
#include "../../detail/date.h"

namespace papryka {
namespace detail {

template <typename _T>
struct RowFilter {
    typedef std::tuple<datetime_t, _T> row_t;
    virtual bool include_row(const datetime_t& date, const _T& row) = 0;
};

template<typename _T>
struct DateRowFilter : RowFilter<_T>
{
    typedef typename RowFilter<_T>::row_t row_t;
    datetime_t from_date;
    datetime_t to_date;

    DateRowFilter(const datetime_t& fromDate, const datetime_t& toDate) :
            from_date(fromDate), to_date(toDate) {}

    bool include_row(const datetime_t& datetime, const _T& row) {
        bool ret = true;
        if (datetime > to_date)
            ret = false;
        if (datetime < from_date)
            ret = false;
        return ret;
    }
};
    
}}