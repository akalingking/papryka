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
 * @file        precision.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 */
#pragma once
#include <cmath>
#include <cstddef>

namespace papryka {

    template <typename _T>
    _T round_(_T x, int precision)
    {
        _T ret = _T(0.0);
        if (precision > 0)
        {
            _T p = _T(std::pow(10, precision));
            _T i = std::floor(x * p + _T(0.5));
            ret = i / p;
        }
        else
        {
            ret = (_T) ((int) x);
        }
        return ret;
    }


    struct precision {
        static const constexpr int s_precision = 6;
        template<typename _T> static _T round(_T value, size_t precision=s_precision) { return round_(value, precision); }
    };

    struct Precision {
        int precision;
        Precision(int precision=6) : precision(precision) {}
        template<typename _T> _T round(_T value) { return round_(value, precision); }
    };

} // namespace papryka
