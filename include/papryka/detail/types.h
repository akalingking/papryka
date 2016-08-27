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
 * @file        types.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        August 19, 2016 8:39 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com> 
 */
#pragma once
#include <cinttypes>
#include <decimal/decimal>
#include "precision.h"
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <iomanip>

#if 0
namespace { 
    static const constexpr int MAX_DEC_LEN = 17;

    inline std::decimal::decimal64 FromString(const char* str)
    {
        if (strlen(str) > MAX_DEC_LEN)
            throw std::runtime_error("bad input");
        char buf[MAX_DEC_LEN+1];
        strncpy(buf, str, MAX_DEC_LEN+1);
        char* point(NULL); 
        if ((point = strchr(buf, '.')) != NULL)
            *(point++) = '\0';
        std::decimal::decimal64 ret(atoi(buf));
        if (point != NULL && *point != '\0')
        {
            int exponent(strlen(point));
            long long unsigned coeff(atoi(point));
            std::decimal::decimal64 d2(std::decimal::make_decimal64(coeff, -exponent));
            if (*buf == '-')
                ret -= d2;
            else
                ret += d2;
        }
        return ret;    
    }
}

// caveat: ostream does not support yet df
inline std::ostream& operator<< (std::ostream& out, const std::decimal::decimal32& value) 
{ 
    double d(std::decimal::decimal_to_double(value));
    return out << std::fixed << std::setprecision(papryka::precision::s_precision) << d;
}
inline std::ostream& operator<< (std::ostream& out, std::decimal::decimal64 const& value) 
{ 
    double d(std::decimal::decimal_to_double(value));
    return out << std::fixed << std::setprecision(papryka::precision::s_precision) << d; 
}
inline std::ostream& operator<< (std::ostream& out, std::decimal::decimal128 const& value) 
{ 
    double d(std::decimal::decimal_to_double(value));
    return out << std::fixed << std::setprecision(papryka::precision::s_precision) << d; 
}
#endif

namespace papryka {
    // @todo: use decimal types, do not use float or double in production
    // @reference  <https://en.wikipedia.org/wiki/Decimal64_floating-point_format>
    // at this point however decimal float is not yet entirely supported by stl (eg. floor, round, abs, ostream, etc.)
    // take a look at precision for workaround using the double and float
    //typedef std::decimal::decimal32 real_t;
    //typedef float real_t;
    typedef double real_t;
}
