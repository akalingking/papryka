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
 * @copyright   (c) <www.sequenceresearch.com>
 */
#pragma once
#include "../detail/types.h"
#include "order.h"

namespace papryka {

    struct Commission {};

    struct NoCommission : Commission {
        template <typename _Order>
        real_t calculate(_Order& order, real_t price, size_t quantity) {
            return 0;
        }
    };

    struct FixedPerTradeCommission : Commission {
        FixedPerTradeCommission(real_t amount) : amount_(amount) {
        }

        template <typename _Order>
        real_t calculate(_Order& order, real_t price, size_t quantity) {

            real_t ret = 0.0;
            if (order.get_execution_info() == nullptr)
                ret = amount_;
            return ret;
        }

    private:
        real_t amount_;
    };


class TradePercentageCommission : Commission
{
public:
    TradePercentageCommission(real_t percent) : percent_(percent) {
    }

    template <typename _Order>
    double calculate(_Order& order, double price, int quantity) {
        return price * quantity * percent_;
    }

private:
    real_t percent_;
};



} // namespace papryka
