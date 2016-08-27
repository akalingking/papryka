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
 * @file        csvfeed.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        August 25, 2016 5:56 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com> 
 */
#pragma once
#include "feed.h"
#include "bar.h"
#include "frequency.h"
#include "../detail/typename.h"
#include "../detail/string.h"
#include <string>
#include <vector>
#include <fstream>
#include <assert>

namespace papryka {

template<typename _T> class FeedCsv;

template<>
class FeedCsv<Bar> : public FeedMemFilter<Bar> 
{
public:
    typedef Bar value_t;
    typedef FeedMemFilter<value_t> base_t;
    typedef typename Traits<value_t>::rows_t rows_t;
    std::string date_column;
    std::string date_format;
    const char delimiter;
    
    inline FeedCsv(const std::string& dateColumn="Date", const std::string& dateFormat="%Y-%m-%d", const char delimiter=',', size_t maxLen=1024, Frequency frequency=Frequency::Day);
    
    inline void add_values_from_csv(const std::string& symbol, const std::string& filename);
};

#include "impl/feedcsv.ipp"
}