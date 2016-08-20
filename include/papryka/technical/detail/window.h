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
 * @file        window.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        August 6, 2016 6:10 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com> 
 */
#pragma once
#include "../../feed/timeseries.h"
#include "../../detail/traits.h"
#include "../../detail/precision.h"
#include <string>

namespace papryka {
namespace detail {

template <typename _T=float> 
class Window
{   
public:    
    typedef _T value_t;
    typedef typename Timeseries<value_t> timeseries_t;
    typedef typename timeseries_t::row_t row_t;
    std::string symbol;
    size_t window_size;
    timeseries_t window;
    timeseries_t out;
    
    bool is_window_full() const { return window_size == window.size(); }
    
    bool empty() const { return out.empty(); }
    
    row_t& operator[](size_t index) { return out[index]; }
    
    const row_t& operator[](size_t index) const { return out[index]; }
    
    const value_t& value(size_t index) const { return std::get<1>(out[index]); }
    
    const datetime_t& datetime(size_t i) const { return std::get<0>(out[index]); }
    
    Event& get_new_value_event() { return out.event_new_value; }
    
    void on_new_value(const datetime_t& datetime, const value_t& value) 
    {
        if (is_window_full())
            window.pop_front();
        window.push_back(typename timeseries_t::row_t(datetime, value));
    }

protected:
    Window(size_t size) : size(size) {}
};

}}