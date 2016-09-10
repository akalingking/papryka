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
 * @file        dispatcher.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 16, 2016 12:04 AM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "subject.h"
#include "event.h"
#include "logger.h"
#include <vector>
#include <memory>
#include <thread>
#include <condition_variable>
#include <exception>
#include <atomic>

namespace papryka {

/**
 * @brief   Event dispatcher
 * @todo    Create separately for backtest and live trading
 */
class Dispatcher
{
public:
    typedef std::thread thread_t;
    typedef std::mutex mutex_t;
    typedef std::condition_variable cond_t;
    typedef std::shared_ptr<Subject> subject_ptr_t;
    typedef std::vector<subject_ptr_t> subjects_t;
    
    Event start_event;
    Event idle_event;
    Event stop_event;
    datetime_t current_date;

    inline Dispatcher();
    inline ~Dispatcher();
    inline bool add_subject(const subject_ptr_t& ptr);
    inline bool stop();
    inline bool start();
    inline void wait();

private:
    inline bool is_run() const;
    inline void do_work();
    inline bool dispatch_subject_(subject_ptr_t& ptr, const datetime_t& currentDate);
    inline bool dispatch_(bool& eof);        
    subjects_t subjects_;
    // slower but safer
    std::atomic<bool> is_run_;
    std::unique_ptr<thread_t> thread_;
    mutex_t mutex_stop_;
    cond_t cond_stop_;
};
    
#include "impl/dispatcher.ipp"
}// namespace papryka
