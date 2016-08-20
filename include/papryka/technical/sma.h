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
 * @file        sma.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        August 17, 2016 5:59 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com> 
 */
#pragma once
#include "detail/window.h"
#include "../feed/timeseries.h"

namespace papryka {

class SMA : public detail::Window<float>
{
public:
    typedef float value_t;
    typedef detail::Window<value_t> base_t;
    typedef Timeseries<value_t> timeseries_t;
    
    SMA(timeseries_t& ts, size_t period) : base_t(period)
    { ts.event_new_value.subscribe(&SMA::on_new_value, this); }
    
    timeseries_t& get_values() { return out; }
        
    void on_new_value(const datetime_t& datetime, const value_t& value)
    {
        value_t first_value = 0.0, current_value = 0.0;
        // get the current first value before it pops out
        // when we insert the most recent value.
        if (window.size() > 0)
            first_value = std::get<1>(window[0]);

        base_t::on_new_value(datetime, value);
        
        if (is_window_full())
        {
            // else condition should be unnecessary
            // provided only as convenience for stochastic oscillator
            // when we are setting the current value to zero
            // and window size is not yet reached. see stochastic oscillator
            if (out.empty() || std::get<1>(out[-1]) == 0.0)
            {
                // normal mean computation
                std::for_each(window.begin(), window.end(), [&](const row_t& row){ current_value += std::get<1>(row); });
                current_value /= window_size; // mean
            }
            else
            {
                // this should be faster
                current_value = std::get<1>(out[-1]);
                current_value = current_value + value / window_size - first_value / window_size;
            }
        }
        out.push_back(typename timeseries_t::row_t(datetime, current_value));
    }
};

}