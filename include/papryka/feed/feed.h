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
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
#pragma once
#include "../detail/date.h"
#include "../detail/subject.h"
#include "../detail/logger.h"
#include "traits.h"
#include "timeseries.h"
#include <memory>
#include <map>

namespace papryka {

    struct Feed : public Subject {
        size_t maxlen;
        Frequency frequency;
        Event new_values_event;
        std::string date_format;
        datetime_t current_date;
    };
    
    template <typename _T=float>
    class FeedBase : public Feed
    {
    public:
        typedef typename Traits<_T>::value_t value_t;
        typedef typename Traits<_T>::values_t values_t;
        typedef Timeseries<_T> ts_t;
        typedef std::shared_ptr<ts_t> ts_ptr_t;
        typedef std::map<std::string, ts_ptr_t> ts_ptrs_t;
        
        values_t current_values;
        
        FeedBase(size_t maxlen, Frequency frequency);
        virtual ~FeedBase();
        bool dispatch();
        virtual void reset();

    protected:
        bool register_timeseries(const std::string& name);
        virtual bool get_next(datetime_t& date, values_t& vals) = 0;

    private:
        bool get_next_and_update(datetime_t& date, values_t& vals);
        ts_ptrs_t ts_ptrs_;
    };

#include "impl/feed.ipp"
} // namespace papryka