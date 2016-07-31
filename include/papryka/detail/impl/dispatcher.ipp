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
 * @file        dispatcher.ipp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 2, 2016 8:41 PM
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
Dispatcher::Dispatcher() : is_stop_(false)
{
}

void Dispatcher::add_subject(const ptr_t& ptr) 
{
    subjects_.push_back(ptr);
}

void Dispatcher::run() 
{
    subjects_t::iterator iter;
    for (iter=subjects_.begin(); iter != subjects_.end(); ++iter)
        (*iter)->start();
    
    start_event.emit();
   
    bool eof, dispatched;
    while (!is_stop()) {
        dispatched = dispatch_(eof);
        if (eof)    
            stop();
        else if (!dispatched)
            idle_event.emit();
    }
    
    for (iter=subjects_.begin(); iter != subjects_.end(); ++iter)
        (*iter)->stop();
    
    stop_event.emit();
}


bool Dispatcher::dispatch_subject_(ptr_t& ptr, const datetime_t& currentDate) 
{
    bool ret = false;
    if (!ptr->eof() && ptr->peek_date() <= currentDate)
        ret = ptr->dispatch();
    return ret;
}

bool Dispatcher::dispatch_(bool& eof) 
{
    bool _eof = true;
    bool _dispatched = false;
    
    datetime_t smallest_date;
    subjects_t::iterator iter;
    for (iter=subjects_.begin(); iter != subjects_.end(); ++iter) 
    {
        ptr_t& ptr = *iter;
        if (!ptr->eof()) 
        {
            _eof = false;
            datetime_t date = ptr->peek_date();
            if (smallest_date == datetime_t())
                smallest_date = date;
            else
                smallest_date = std::min(smallest_date, date);
        } 
    }
    
    current_date = smallest_date;
    
    if (!_eof) 
    {
        for (iter=subjects_.begin(); iter != subjects_.end(); ++iter) 
        {
            ptr_t& ptr = *iter;
            if (dispatch_subject_(ptr, smallest_date))
                _dispatched = true;
        }
    }
    
    eof = _eof;
    
    return _dispatched;
}
