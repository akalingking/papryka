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
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
Dispatcher::Dispatcher() : is_run_(false)
{
    log_debug("Dispatcher::{}", __func__);
}

Dispatcher::~Dispatcher()
{
    log_trace("Dispatcher::{} entry", __func__);
     
    if (is_run())
        stop();
    
    if (thread_) 
    {
        if (thread_->joinable())
            thread_->join();
        else
            thread_->detach();
    }
    
    log_trace("Dispatcher::{} exit", __func__);
}

bool Dispatcher::start() 
{ 
    if (!is_run_.load())
    {
        is_run_.store(true);
        thread_.reset(new thread_t(&Dispatcher::do_work, this));
    }
    return is_run_;
}

bool Dispatcher::stop() 
{ 
    if (is_run_.load())
        is_run_.store(false);
    return !is_run_.load();
}

bool Dispatcher::is_run() const
{
    return is_run_.load();
}

void Dispatcher::run() 
{
    if (start())
        wait();
}

void Dispatcher::wait()
{
    std::unique_lock<mutex_t> lock(mutex_stop_);
    cond_stop_.wait(lock);
}

bool Dispatcher::add_subject(const subject_ptr_t& ptr) 
{
    bool ret = false;
    if (!is_run_.load())
    {
        subjects_.push_back(ptr);
        ret = true;
    }
    return ret;
}

void Dispatcher::do_work() 
{
//    log_debug("Dispatcher::{} (entry)", __func__);
    try 
    {
        subjects_t::iterator iter;
        for (iter=subjects_.begin(); iter != subjects_.end(); ++iter)
            (*iter)->start();
        
        start_event.emit();
        
        bool eof, dispatched;
        while (is_run_.load()) 
        {
            dispatched = dispatch_(eof);
            if (eof) 
            {    
                is_run_.store(false);
                break;
            }
            else if (!dispatched)
            {
                idle_event.emit();
            }
        }

        for (iter=subjects_.begin(); iter != subjects_.end(); ++iter)
            (*iter)->stop();

        stop_event.emit();
    }
    catch (std::exception& e)
    {
        log_error("Dispatcher::{} exception='{}'", __func__, e.what());
    }
    catch (...)
    {
        log_error("Dispatcher::{} unknown exception", __func__);
    }
    
    log_trace("Dispatcher::{} thread ended", __func__);
    
    std::unique_lock<mutex_t> lock(mutex_stop_);
    cond_stop_.notify_all();
    
//    log_debug("Dispatcher::{} (exit)", __func__);
}

bool Dispatcher::dispatch_subject_(subject_ptr_t& ptr, const datetime_t& currentDate) 
{
//    log_trace("Dispatcher::{} (entry)", __func__);
    
    bool ret = false;
    assert (ptr);
    if (!ptr->eof() && ptr->peek_date() <= currentDate)
        ret = ptr->dispatch();
    
//    log_trace("Dispatcher::{} (exit)", __func__);
    
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
        subject_ptr_t& ptr = *iter;
        assert (ptr);

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
            subject_ptr_t& ptr = *iter;
            assert (ptr);
            if (dispatch_subject_(ptr, smallest_date))
                _dispatched = true;
        }
    }
    
    eof = _eof;
    
    return _dispatched;
}
