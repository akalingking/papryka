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
 * @file        bartimeseries.ipp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
BarTimeseries::BarTimeseries(
    Frequency frequency,
    size_t maxlen,
    Event* eventNewValue) :
    Timeseries(frequency, maxlen, eventNewValue)
{
}

void BarTimeseries::push_back(const row_t& row)
{
    base_t::push_back(row);
    const datetime_t& datetime = std::get<0>(row);
    const Bar& bar = std::get<1>(row);
    open.push_back(timeseries_t::row_t(datetime, bar.open));
    high.push_back(timeseries_t::row_t(datetime, bar.high));
    low.push_back(timeseries_t::row_t(datetime, bar.low));
    close.push_back(timeseries_t::row_t(datetime, bar.close));
    volume.push_back(timeseries_t::row_t(datetime, bar.volume));
}
