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
 * @file        papryka.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 */
#pragma once
#include <gnuplot-iostream.h>
#include <paprika/detail/date.h>
#include <papryka/detail/event.h>
#include <papryka/feed/timeseries.h>

namespace papryka {
namespace util {

class Subplot
{
public:
typedef std::shared_ptr<Timeseries> ts_ptr_t;
typedef std::map<std::string, ts_ptr_t> ts_ptrs_t;

virtual void on_bars(Strategy& strategy, const Bars& bars) {}

};

} // namespace util
} // namespace papryka