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
 * @file        slippage.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
#pragma once
#include "../detail/types.h"
#include "../feed/bar.h"
#include <cassert>

namespace papryka {

struct NoSlippage
{

    template <typename _Order, typename _T>
    real_t calculate_price(_Order& order, const _T& value, real_t price, size_t quantity, real_t volumeUsed)
    {
        return price;
    }
};

} // namespace papryka
