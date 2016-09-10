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
 * @file        strategy.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        August 7, 2016 1:59 AM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com> 
 */
#include <gtest/gtest.h>
#include <papryka/papryka.h>
#include <papryka/exchange/exchange.h>
#include <papryka/feed/feedsynthetic.h>
#include <papryka/strategy/strategy.h>
#include <exception>
#include <string>
#include <cassert>

using namespace papryka;

class StrategyTest : public Strategy<StrategyTest, Bar>
{
public:
    typedef Strategy<StrategyTest, Bar> base_t;
    typedef base_t::exchange_ptr_t exchange_ptr_t;
    typedef base_t::value_t value_t;
    typedef base_t::values_t values_t;
    std::string symbol;
    
    StrategyTest(exchange_ptr_t exchange, const std::string& symbol) : base_t(exchange), symbol(symbol)
    {
        log_debug("StrategyTest::{}", __func__);
    }
    
    ~StrategyTest() 
    { 
        log_debug("StrategyTest::{}", __func__); 
    }
    
    void on_start()
    {
        log_debug("StrategyTest::{}", __func__);
    }
    
    void on_idle()
    {
        log_debug("StrategyTest::{}", __func__);
    }
    
    void on_stop()
    {
        log_debug("StrategyTest::{}", __func__);
    }
    
    void on_order_updated(order_t* order)
    {
        log_debug("StrategyTest::{} order id={}", __func__, order->id);
    }
    
    void on_bars(const datetime_t& datetime, const values_t& bars)
    {     
        try
        {
            const value_t& bar = bars.at(symbol);
            log_debug("StrategyTest::{} {}", __func__, bar);
        }
        catch (std::exception& e)
        {
            log_error("StrategyTest::{} {}", __func__, e.what());
        }
    } 
};

TEST(Strategy, basicConstruction)
{
    typedef FeedSynthetic<Bar> feed_t;
    typedef Exchange<Bar> exchange_t;
    typedef exchange_t::ptr_t exchange_ptr_t;
    typedef StrategyTest strategy_t;
    typedef std::shared_ptr<strategy_t> strategy_ptr_t;
    
    // 1. Mark stock symbol
    std::string symbol = "GOOG";

    // 2. Create date interval of 100 days
    datetime_t end = Clock::now();
    datetime_t start = end - std::chrono::days(100);

    // 3. Construct data feed
    std::shared_ptr<feed_t> feed(new feed_t(start, end, Frequency::Day));

    // 4.a. Construct spot bar price
    Bar goog_spot(771, 773, 770, 772, 0, 1350000);
    // 4.b. Set data table { symbol, spot price, volatility, expected return } 
    feed_t::data_t data = {{"GOOG", goog_spot, 0.2, 0.1}};
    // 4.c. Generate synthetic data
    feed->add_values_from_generator(data);

    // 5. Construct exchange and set start equity
    exchange_ptr_t exchange(new exchange_t(feed, 1000));

    // 6. Build strategy
    strategy_ptr_t strategy(new strategy_t(exchange, symbol)); 

    // 7. Run simulation
    strategy->run();
    EXPECT_EQ(0,0);
}