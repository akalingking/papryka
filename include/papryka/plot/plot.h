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
 * @file        papryka.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 */
#pragma once
#include <gnuplot-iostream.h>
#include <papryka/detail/date.h>
#include <papryka/detail/event.h>
#include <papryka/exchange/order.h>
#include <papryka/feed/bar.h>
#include <string>
#include <map>

namespace papryka {

    
class SubplotBase
{
protected:
    // put virtual functions here
};

template <typename _Bar> class Exchange;
template <typename _Derived> class Strategy;
template <typename _Bar> class Timeseries;

template <typename _Strategy, typename _Bar>
class Subplot
{
public:
    typedef Timeseries<_Bar> Timeseries_t;
    typedef std::shared_ptr<Timeseries_t> ts_ptr_t;
    typedef std::map<std::string, ts_ptr_t> ts_ptrs_t;

    // fix virtual functions here
    virtual void on_bars(Strategy<_Strategy>& strategy, const Bar& bars) {}

    virtual void on_order_event(Exchange<_Bar>& exchange, std::shared_ptr<Order<> > orderEvent) {}

    Timeseries_t* add_series(const std::string& name, Event*);

    void remove_series(const std::string&);

    Timeseries_t* get_series(const std::string&);

    virtual void set_use_adj_values(bool) { assert(false); }

    virtual void plot(Gnuplot&);

    virtual void plot(Gnuplot&, const datetime_t&, const datetime_t&) = 0;

    virtual void set_plot_buy_sell(bool) = 0;

    virtual void set_plot_volume(bool) = 0;

protected:
    Subplot(const std::string& name);
    std::string name_;
    ts_ptrs_t series_;
};

#include "./ipp/plot.ipp"

} // namespace papryka
