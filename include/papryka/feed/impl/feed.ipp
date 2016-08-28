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
 * @file        feed/feed.ipp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 2, 2016 8:41 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
template<typename _T>
Feed<_T>::Feed(Frequency frequency, size_t maxLen) :
        maxlen(maxLen), frequency(frequency), date_format(frequency==Frequency::Day?s_date_format:s_datetime_ms_format), is_closeadj_on(false)
{
    log_trace("Feed<_T>::{}", __func__);
}

template<typename _T>
Feed<_T>::~Feed() 
{ 
    log_trace("Feed<_T>::{}", __func__);
}

template<typename _T>
void Feed<_T>::reset() 
{
    std::vector<std::string> keys;
    for (typename ts_ptrs_t::value_type& val : ts_ptrs_)
        keys.push_back(val.first);
    
    ts_ptrs_.clear();
    
    for (typename std::string& key : keys)
        register_timeseries(key);
}

template<typename _T>
bool Feed<_T>::dispatch() 
{
    
    log_trace("Feed<_T::{} (entry)", __func__);
    
    datetime_t datetime;
    values_t values;
    get_next_and_update(datetime, values);
    if (datetime != nulldate) 
    {
        current_date = datetime;
#ifdef _DEBUG
        for (const typename values_t::value_type& item : values) {
            std::stringstream strm;
            strm << item.second;
            std::string str = strm.str();
            log_debug("Feed<_T>::{} {} {} {}", __func__, papryka::to_str(datetime), item.first, str);
        }
#endif
        new_values_event.emit(datetime, values);
    }
    
    log_trace("Feed<_T>::{} (exit)", __func__);
    
    return true;
}

template<typename _T>
bool Feed<_T>::register_timeseries(const std::string& name) 
{
    log_trace("Feed<_T>::{} (entry)", __func__);
    
    bool ret = false;
    typename ts_ptrs_t::iterator iter= ts_ptrs_.find(name);
    if (iter == ts_ptrs_.end()) 
    {
        ts_ptr_t ptr(new ts_t(frequency, maxlen));
        ts_ptrs_.insert(typename ts_ptrs_t::value_type(name, ptr));
        ret = true;
    }
    
    log_trace("Feed<_T>::{} (exit)", __func__);
    return ret;
}

template<typename _T>
bool Feed<_T>::get_next_and_update(datetime_t& date, values_t& values) 
{
    log_trace("Feed<_T>::{} (entry)", __func__);    
    bool ret = false;
    if (get_next(date, values)) 
    {
        current_values = values;
        if (date != nulldate) 
        {
            typename values_t::iterator iter = values.begin();
            for (;iter != values.end(); ++iter) 
            {
                const std::string& symbol = iter->first;
                const value_t& value = iter->second;
                ts_t* ts = nullptr;
                
                typename ts_ptrs_t::iterator iter = ts_ptrs_.find(symbol);
                if (iter != ts_ptrs_.end()) 
                {
                    ts = iter->second.get();    
                } 
                else 
                {
                    // timeseries is not registered, silently accept.
                    register_timeseries(symbol);
                    iter = ts_ptrs_.find(symbol);
                    ts = iter->second.get();
                }
                
                assert(ts);
                ts->push_back(typename ts_t::row_t(date, value));
                ret = true;
            }
        }
    }
    log_trace("Feed<_T>::{} (exit)", __func__);
    return ret;
}
