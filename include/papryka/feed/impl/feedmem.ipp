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
 * @date        July 2, 2016 8:41 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
template<typename _T>
FeedMem<_T>::FeedMem(size_t maxlen, Frequency frequency) :
    Feed<_T>(maxlen, frequency),
    is_started_(false)
{
    log_debug("FeedMem CREATED");
}

template<typename _T>
bool FeedMem<_T>::add_values(const std::string& name, const rows_t& rows)
{
    log_trace("FeedMem::{} entry", __func__);
    
    if (is_started_)
    {
        log_error("FeedMem::{} feed for {} is already started",  __func__, name.c_str());
        return false;
    }
    
    typename map_rows_t::iterator iter = map_rows_.find(name);
    if (iter == map_rows_.end())
    {
        Feed<_T>::register_timeseries(name);
        map_rows_.insert(typename map_rows_t::value_type(name, rows));
    }
    else
    {
        for (size_t i=0; i<rows.size();++i)
            iter->second.push_back(rows[i]);
    }
    
    assert (!map_rows_.empty());
    assert (map_rows_.find(name) != map_rows_.end());
    values_left_[name] = map_rows_[name].size();
    next_indexes_[name] = 0;
    
    log_trace("FeedMem::{} exit", __func__);
    return true;
}

template<typename _T>
void FeedMem<_T>::start() {
    is_started_ = true;
}

template<typename _T>
void FeedMem<_T>::stop() {
    is_started_ = false;
}

template<typename _T>
void FeedMem<_T>::reset() {
    is_started_ = false; 
    Feed<_T>::reset();
}

template<typename _T>
bool FeedMem<_T>::eof() {
//    log_trace("FeedMem::eof entry");
    bool ret = true;
    typename map_rows_t::iterator iter;
    for (iter = map_rows_.begin(); iter != map_rows_.end(); ++iter)
    {
        const std::string& name = iter->first;
        rows_t& rows = iter->second;
//        log_debug("FeedMem::eof symbol={} size={}", name, rows.size());
        size_t next_index = next_indexes_[name];
        if (next_index < rows.size())
        {
            ret = false;
            break;
        }
    }
//    log_trace("FeedMem::eof exit");
    return ret;
}

template<typename _T>
datetime_t FeedMem<_T>::peek_date() {
    datetime_t date;
    size_t next_index = 0;
    typename map_rows_t::iterator iter;
    for (iter = map_rows_.begin(); iter != map_rows_.end(); ++iter)
    {
        const std::string& name = iter->first;
        rows_t& rows = iter->second;
        
        next_index = next_indexes_[name];
        if (next_index < rows.size())
        {
            if (date == nulldate) 
                date = std::get<0>(rows[next_index]);
            else
                date = std::min(date, std::get<0>(rows[next_index]));
        }
    }
    
    return date;
}

template<typename _T>
bool FeedMem<_T>::get_next(datetime_t& date, values_t& values)
{
    log_trace("FeedMem::get_next entry");
    
    bool ret = false;
    date = this->peek_date();
    
    if (date == nulldate) {
        log_debug("FeedMem::{} smallest date is null", __func__);
        return ret;
    }
     
    if (!this->eof()) {
        typename map_rows_t::iterator iter;
        for (iter = map_rows_.begin(); iter != map_rows_.end(); ++iter) {
            const std::string& name = iter->first;
            rows_t& rows = iter->second;
            
            size_t next_index = next_indexes_[name];
            if (next_index < rows.size())
            {
                row_t& row = rows[next_index];
                const datetime_t& date_ = std::get<0>(row);
                if (date_ == date)
                {
                    value_t& val = std::get<1>(row);                    
                    values.insert(typename values_t::value_type(name, val));
                    ++(next_indexes_[name]);
                    --(values_left_[name]);
                }
            }
        }
        ret = true;
    }
    log_trace("FeedMem::get_next exit");
    return ret;
}
