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
 * @file        bar.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 23, 2016 6:17 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include <ostream>

namespace papryka {
    struct Bar {
        Bar(float open=0.0, float high=0.0, float low=0.0, float close=0.0, float volume=0.0) :
                open(open), high(high), low(low), close(close), volume(volume) {} 
        float open;
        float high;
        float low;
        float close;
        float closeadj;
        float volume;
    };

#ifdef _DEBUG    
    inline std::ostream& operator<<(std::ostream& os, const Bar& bar) {
        os << "o:"<<bar.open << " h:" << bar.high << " l:" << bar.low << " c:" << bar.close << " v:" << bar.volume;
        return os;
    }
#endif
}