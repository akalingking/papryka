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
 * @file        feed.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 9, 2016 1:37 AM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "../detail/date.h"
#include "../detail/subject.h"
#include "../detail/logger.h"
#include "../detail/traits.h"
#include "detail/filter.h"
#include "timeseries.h"
#include <memory>
#include <map>

namespace papryka {

template <typename _T=float>
class Feed : public Subject
{
public:
    typedef std::shared_ptr<Feed<_T>> ptr_t;
    typedef typename Traits<_T>::value_t value_t;
    typedef typename Traits<_T>::values_t values_t;
    typedef Timeseries<_T> ts_t;
    typedef std::shared_ptr<ts_t> ts_ptr_t;
    typedef std::map<std::string, ts_ptr_t> ts_ptrs_t;

    values_t current_values;
    datetime_t current_date;
    Event new_values_event;
    std::string date_format;
    size_t maxlen;
    Frequency frequency;

    Feed(size_t maxlen, Frequency frequency);
    virtual ~Feed();
    bool dispatch();
    virtual void reset();

protected:
    bool register_timeseries(const std::string& name);
    virtual bool get_next(datetime_t& date, values_t& vals) = 0;

private:
    bool get_next_and_update(datetime_t& date, values_t& vals);
    ts_ptrs_t ts_ptrs_;
};

/**
 * @brief Memory cached feed
 * @param _T bar type (e.g. float,bar, quotes)
 */
template <typename _T=float>
class MemFeed : public Feed<_T>
{
public:
    typedef _T value_t;
    typedef typename Traits<value_t>::values_t values_t;
    typedef typename Traits<value_t>::row_t row_t;
    typedef typename Traits<value_t>::rows_t rows_t;
    typedef std::map<std::string, size_t> indexes_t;
    typedef std::map<std::string, rows_t> map_rows_t;

    MemFeed(size_t maxlen=1024, Frequency frequency=Frequency::Day);
    bool add_values(const std::string& name, const rows_t& rows);
    bool get_next(datetime_t& date, values_t& values);
    //@{from subject
    void start();
    void stop();
    bool eof();
    void reset();
    datetime_t peek_date();
    //@}from subject

private:
    bool is_started_;
    map_rows_t map_rows_;
    indexes_t next_indexes_;
    indexes_t values_left_;
};

template <typename _T>
class FilterMemFeed : public MemFeed<_T> 
{
protected:
    std::shared_ptr<detail::RowFilter> filter_;
    FilterMemFeed(int maxlen, Frequency frequency) : MemFeed<_T>(maxlen, frequency) 
    { log_trace("FilterMemFeed created"); }    
public:
    void set_date_range(const datetime_t& from, const datetime_t& to) 
    { filter_.reset(new detail::DateRowFilter(from, to)); }
};

#include "impl/feed.ipp"
#include "impl/memfeed.ipp"
}