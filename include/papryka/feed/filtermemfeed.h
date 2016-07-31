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
 * @file        filtermemfeed.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 16, 2016 5:19 PM
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
#pragma once
#include "../detail/date.h"
#include "memfeed.h"
#include "filter.h"
#include <memory>

namespace papryka {

    template <typename _T>
    class FilterMemFeed : public MemFeed<_T> 
    {
    protected:
        std::shared_ptr<RowFilter> filter_;
        FilterMemFeed(int maxlen, Frequency frequency) : MemFeed<_T>(maxlen, frequency) {
                log_trace("FilterMemFeed created");
        }    
    public:
        void set_date_range(const datetime_t& from, const datetime_t& to) { 
            filter_.reset(new DateRowFilter(from, to)); 
        }
    };
    
} // namespace papryka