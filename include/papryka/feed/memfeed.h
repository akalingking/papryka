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
 * @file        memfeed.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 16, 2016 1:18 AM
 * @copyright   2017 www.sequenceresearch.com
 */
#pragma once
#include "feed.h"
#include "timeseries.h"
#include "../detail/logger.h"

namespace papryka {

    template <typename _T=float>
    class MemFeed : public FeedBase<_T>
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

    #include "impl/memfeed.ipp"

} // namespace papryka