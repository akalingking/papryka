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
 * @date        July 23, 2016 7:51 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#pragma once
#include "../feed/bar.h"
#include <cassert>

namespace papryka {

struct NoSlippage
{

    template <typename _Order, typename _T>
    float calculate_price(_Order& order, const _T& value, float price, float quantity, float volumeUsed)
    {
        return price;
    }
};

/**
 * A volume share slippage model as defined in Zipline's VolumeShareSlippage model.
 * The slippage is calculated by multiplying the price impact constant by the 
 * square of the ratio of the order to the total volume.
 * Check https://www.quantopian.com/help#ide-slippage for more details.
 */
//    struct VolumeShareSlippage
//    {
//        typedef Bar value_t;
//        typedef Order<value_t> order_t;
//        float price_impact;
//        VolumeShareSlippage(float priceImpact=0.1) : price_impact(priceImpact) {}
//
//        float calculate_price(order_t& order, float price, float quantity, const value_t& value, float volumeUsed) 
//        {
//            float volume = value.volume;
//            assert (volume != 0.0);
//            float total_volume = volumeUsed + quantity;
//            float volume_share = total_volume / volume;
//            float impact_pcnt = (volume_share*volume_share)*price_impact;
//
//            float ret = 0.0;
//            if (order.is_buy())
//                ret = price * (1 + impact_pcnt);
//            else
//                ret = price * (1 - impact_pcnt);
//            
//            return ret;
//        }
//    };
} // namespace papryka
