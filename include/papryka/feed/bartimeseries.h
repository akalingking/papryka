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
 * @file        bartimeseries.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 */
#pragma once
#include <papryka/detail/date.h>
#include <papryka/detail/event.h>
#include <papryka/feed/bar.h>
#include <papryka/feed/timeseries.h>
#include <memory>
#include <vector>
#include <map>
#include <cassert>

namespace papryka {

/**
 * Consider making a template class that
 * auto generates the underlying fields.
 */
class BarTimeseries : public Timeseries<Bar>
{
public:
    typedef Timeseries<Bar> base_t;
    typedef base_t::row_t row_t;
    typedef Timeseries<real_t> timeseries_t;
    timeseries_t open;
    timeseries_t high;
    timeseries_t low;
    timeseries_t close;
    timeseries_t volume;
    BarTimeseries( Frequency frequency=Frequency::Day, size_t maxlen=1024, Event* eventNewValue=nullptr);
    void push_back(const row_t& row);
};

#include "impl/bartimeseries.ipp"
}

