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
 * @file        traits.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 23, 2016 8:15 PM
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
#pragma once
#include "../detail/date.h"
#include <vector>
#include <string>
#include <deque>

namespace papryka {
    template <typename _T=float, typename _Alloc=std::allocator<_T>>
    struct Traits {
        typedef _T value_t;
        typedef std::map<std::string, value_t> values_t;
        typedef std::tuple<datetime_t, value_t> row_t;
        typedef std::deque<row_t, _Alloc> rows_t;
     };
} // namespace papryka
