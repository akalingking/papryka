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
 * @file        timeseries.ipp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */

template<typename _T, typename _Alloc>
Timeseries<_T,_Alloc>::Timeseries(Frequency frequency, size_t maxlen, Event* eventSource) :
    frequency(frequency), maxlen(maxlen)
{
    if (eventSource)
        eventSource->subscribe(&Timeseries::on_new_value_, this);
    log_trace("Timeseries<_T>::{} max={}", __func__, maxlen);
}

template<typename _T, typename _Alloc>
Timeseries<_T,_Alloc>::~Timeseries()
{
    log_trace("Timeseries<_T>::{} size={}", __func__, rows_.size());
}

template<typename _T, typename _Alloc>
void Timeseries<_T,_Alloc>::push_back(const row_t& row)
{
    if (rows_.size() < maxlen)
    {
        if (!rows_.empty() && std::get<0>(row) != nulldate)
        {
            const row_t& last = rows_.back();
            if (std::get<0>(last) < std::get<0>(last))
                throw std::logic_error("invalid date");
        }

        rows_.push_back(row);
        event_new_value.emit(row);
    }
    else
    {
        std::string error("max size error, ");
        error += std::to_string(rows_.size());
        error += " > ";
        error += std::to_string(maxlen);
        //throw std::out_of_range("max range");
        throw std::out_of_range(error);
    }
}

template<typename _T, typename _Alloc>
typename Timeseries<_T,_Alloc>::row_t Timeseries<_T,_Alloc>::pop_front()
{
    row_t ret;
    if (!rows_.empty())
    {
         ret = rows_.front();
         rows_.pop_front();
    }
    return ret;
}

template<typename _T, typename _Alloc>
const typename Timeseries<_T,_Alloc>::row_t&
Timeseries<_T,_Alloc>::operator[](size_t i) const
{
    size_t size = (int)rows_.size();

    if ((i > 0 && i >= size) || (i < 0 && (i+size) < 0) || size == 0)
        throw std::out_of_range("out of range");

    return (i < 0) ? rows_[i+size] : rows_[i];
}

template<typename _T, typename _Alloc>
typename Timeseries<_T,_Alloc>::row_t&
Timeseries<_T,_Alloc>::operator[](size_t i)
{
    size_t size = (int)rows_.size();

    if ((i > 0 && i >= size) || (i < 0 && (i+size) < 0) || size == 0)
        throw std::out_of_range("out of range");

    return (i < 0) ? rows_[i+size] : rows_[i];
}

template<typename _T, typename _Alloc>
void Timeseries<_T,_Alloc>::on_new_value_(const row_t& row)
{
    this->push_back(row);
}

template<typename _T, typename _Alloc>
size_t Timeseries<_T,_Alloc>::size() const
{
    return rows_.size();
}

template<typename _T, typename _Alloc>
void Timeseries<_T,_Alloc>::clear()
{
    rows_.clear();
}

template<typename _T, typename _Alloc>
bool Timeseries<_T,_Alloc>::empty() const
{
    return rows_.empty();
}

template<typename _T, typename _Alloc>
typename Timeseries<_T,_Alloc>::iterator_t&
Timeseries<_T,_Alloc>::begin()
{
    rows_.begin();
}

template<typename _T, typename _Alloc>
typename Timeseries<_T,_Alloc>::const_iterator_t&
Timeseries<_T,_Alloc>::begin() const
{
    rows_.begin();
}

template<typename _T, typename _Alloc>
typename Timeseries<_T,_Alloc>::iterator_t&
Timeseries<_T,_Alloc>::end()
{
    rows_.end();
}

template<typename _T, typename _Alloc>
typename Timeseries<_T,_Alloc>::const_iterator_t&
Timeseries<_T,_Alloc>::end() const
{
    rows_.end();
}
