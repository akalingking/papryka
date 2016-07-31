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
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
#pragma once
#include "subject.h"
#include "event.h"
#include <vector>
#include <memory>

namespace papryka {
    
    class Dispatcher
    {
    public:
        typedef std::shared_ptr<Subject> ptr_t;
        typedef std::vector<ptr_t> subjects_t;
        Event start_event;
        Event idle_event;
        Event stop_event;
        datetime_t current_date;
        
        Dispatcher();

        void run();
        
        void add_subject(const ptr_t& ptr);
        
        inline void stop() { is_stop_ = true; }
        
        inline bool is_stop() const { return is_stop_; }

    private:
        bool dispatch_subject_(ptr_t& ptr, const datetime_t& currentDate);
        
        bool dispatch_(bool& eof);        
        
        subjects_t subjects_;
        bool is_stop_;
    };
    
#include "impl/dispatcher.ipp"
}// namespace papryka