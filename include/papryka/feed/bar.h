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
#include "../detail/types.h"
#include <ostream>

namespace papryka {
    
    struct Bar {
        explicit Bar(real_t open=real_t(0.0), real_t high=real_t(0.0), real_t low=real_t(0.0), real_t close=real_t(0.0), real_t closeAdj=real_t(0.0), real_t volume=real_t(0.0)) :
                open(open), high(high), low(low), close(close), closeadj(closeAdj), volume(volume)  
        {
             if (high < low)
                throw std::logic_error("High < Low");
            else if (high < open)
                throw std::logic_error("High < Open");
            else if (high < close)
                throw std::logic_error("High < Close");
            else if (low > open)
                throw std::logic_error("Low > Open");
            else if (low > close)
                throw std::logic_error("Low > Close");
        } 

        real_t open;
        real_t high;
        real_t low;
        real_t close;
        real_t closeadj;
        real_t volume;
    };
    
    inline real_t get_adjusted(real_t value, real_t close, real_t adjClose) 
    {
        if (value ==0) return 0;
        if (adjClose != 0)
            return (adjClose * (value/close));
        else
            throw std::logic_error("adjclose is 0");
    }
    
    inline real_t get_typical_price(const Bar& bar) 
    { 
        return (bar.high + bar.low + bar.close) / 3;
    }

    inline std::ostream& operator<<(std::ostream& os, const Bar& bar)
    {
        os << std::fixed << std::setprecision(papryka::precision::s_precision) << "o:" << bar.open << " h:" << bar.high << " l:" << bar.low << " c:" << bar.close  << " adj:" << bar.closeadj << " v:"  << bar.volume;
        return os;
    }
}