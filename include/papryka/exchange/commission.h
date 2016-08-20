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
 * @file        commission.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 23, 2016 7:56 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "order.h"

namespace papryka {

    struct NoCommission {
        template <typename _Order>
        float calculate(_Order& order, double price, int quantity) {
            return 0;
        }
    };
#if 0
    /**
     * Charge commission only on first fill
     */
    struct FixedPerTradeCommission
    {
        float amount;
        FixedPerTradeCommission(double amount) : amount(amount) {}
        float calculate(Order& order, float price, int quantity) {
            float ret = 0.0;
            if (order.info == nullptr)
                ret = amount;
            return ret;
        }
    };

    struct TradePercentageCommission {
        float percent;
        TradePercentageCommission(float percent) : percent(percent) {}
        float calculate(Order& order, float price, int quantity) {
            return price * quantity * percent;
        }
    };
#endif

} // namespace papryka