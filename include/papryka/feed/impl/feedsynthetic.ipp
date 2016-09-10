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
 * @file        bartimeseries.ipp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 2, 2016 8:41 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */ 
//@{ Basic Random Generator
Generator_::Generator_(const datetime_t& start, const datetime_t& end, Frequency frequency) :
        GeneratorBase(start, end, frequency), dist(0.1, 1.0)
{
    rng.seed(50000);
    log_trace("Generator_ created");
}

BasicGenerator<real_t>::BasicGenerator(const datetime_t& start, const datetime_t& end, Frequency frequency) :
        Generator_(start, end, frequency) {
    log_trace("BasicGenerator created");
}

size_t BasicGenerator<real_t>::generate(rows_t& rows) 
{
    log_trace("BasicGenerator::generate entry");
    datetime_t date = start;
    ql::Calendar cal = ql::UnitedStates();
    size_t counter = 0;
    while (date <= end && counter++ < maxlen) 
    {
        if (cal.isBusinessDay(to_qdate(date))) 
        {
            real_t value = (real_t)dist(rng);
            row_t row = row_t(date, value);
            rows.push_back(row);
        }
        date = get_next_timepoint(date, frequency);
    }
    log_trace("BasicGenerator::generate exit");
    return counter;
}

BasicGenerator<Bar>::BasicGenerator(const datetime_t& start, const datetime_t& end, Frequency frequency) :
        Generator_(start, end, frequency) 
{ log_trace("BasicGenerator created"); }

size_t BasicGenerator<Bar>::generate(rows_t& rows) 
{
    log_trace("BasicGenerator::generate entry");
    datetime_t date = start;
    ql::Calendar cal = ql::UnitedStates();
    size_t counter = 0;
    while (date <= end && counter++ < maxlen) 
    {
        if (cal.isBusinessDay(to_qdate(date))) 
        {
            std::vector<real_t> vals;
            vals.push_back(real_t(dist(rng)));
            vals.push_back(real_t(dist(rng)));
            vals.push_back(real_t(dist(rng)));
            vals.push_back(real_t(dist(rng)));
            std::sort(vals.begin(), vals.end());

            value_t value;
            value.open = vals[1];
            value.high = vals[3];
            value.low = vals[0];
            value.close = vals[2];
            value.closeadj = real_t(dist(rng));
            value.volume = real_t(dist(rng)*1000000);
            row_t row = row_t(date, value);
            rows.push_back(row);
        }
        date = get_next_timepoint(date, frequency);
    }
    log_trace("BasicGenerator::generate exit");
    return counter;
}
//@} Basic Random Generator

//@{ Geometric Brownian Motion Generator
template <typename _T>
GbmGenerator<_T>::GbmGenerator(const datetime_t& start, const datetime_t& end, const value_t& startPrice, real_t volatilityPct, real_t meanReturnPct, Frequency frequency) :
        GeneratorBase(start, end, frequency), impl(startPrice, volatilityPct, meanReturnPct, 0.0, get_bars_per_day(frequency)) 
{   
}

template <typename _T>
size_t GbmGenerator<_T>::generate(rows_t& rows) 
{
    log_trace("GbmGenerator::generate entry");
    datetime_t date = start;
    ql::Calendar cal = ql::UnitedStates();
    size_t counter = 0;
    while (date <= end && counter++ < maxlen) 
    {
        if (cal.isBusinessDay(to_qdate(date))) 
        {
            value_t value = impl.generate();
            log_trace("GbmGenerator<{}>::{} {} value:{}", type_name<_T>(), __func__,to_str(date), value);
            row_t row = row_t(date, value);
            rows.push_back(row);
        }
        date = get_next_timepoint(date, frequency);
    }
    log_trace("GbmGenerator::generate exit");
    return counter;
}

GbmGenerator<Bar>::GbmGenerator(const datetime_t& start, const datetime_t& end, const Bar& startPrice, real_t volatilityPct, real_t meanReturnPct, Frequency frequency) :
        GeneratorBase(start, end, frequency),
        open(startPrice.open, volatilityPct, meanReturnPct, 0.0, get_bars_per_day(frequency)),
        high(startPrice.high, volatilityPct, meanReturnPct, 0.0, get_bars_per_day(frequency)),
        low(startPrice.low, volatilityPct, meanReturnPct, 0.0, get_bars_per_day(frequency)),
        close(startPrice.close, volatilityPct, meanReturnPct, 0.0, get_bars_per_day(frequency)),
        volume(startPrice.volume, volatilityPct, meanReturnPct, 0.0, get_bars_per_day(frequency))
{
}
    
size_t GbmGenerator<Bar>::generate(rows_t& rows) 
{
    log_trace("GbmGenerator::generate entry");
    datetime_t date = start;
    ql::Calendar cal = ql::UnitedStates();
    size_t counter = 0;
    value_t value;
    while (date <= end && counter++ < maxlen) 
    {
        if (cal.isBusinessDay(to_qdate(date))) 
        {
            // fill open, close and volume
            value.open = open.generate();
            value.close = close.generate();
            value.volume = volume.generate();
            // fill vector to find high and low
            std::vector<real_t> values;
            values.push_back(value.open);
            values.push_back(value.close);
            values.push_back(low.generate());
            values.push_back(high.generate());
            
            std::sort(values.begin(), values.end());
            value.low = values.front();
            value.high = values.back();
            log_trace("GbmGenerator<Bar>::{0:} {1:} o:{2:0.3f} h:{3:0.3f} l:{4:0.3f} c:{5:0.3f} v:{6:}", 
                    __func__,to_str(date), value.open,value.high,value.low,value.close,value.volume);
            row_t row = row_t(date, value);
            rows.push_back(row);
        }        
        date = get_next_timepoint(date, frequency);
    }
    log_trace("GbmGenerator::generate exit");
    return counter;
}
//@} Geometric Brownian Motion Generator

template<typename _T, typename _Generator>
FeedSynthetic<_T, _Generator>::FeedSynthetic(datetime_t start, const datetime_t& end, Frequency frequency, size_t maxlen) :
        base_t(frequency, maxlen), start_(start), end_(end)
{
    log_debug("FeedSynthetic::{}", __func__);
}

template<typename _T, typename _Generator>
FeedSynthetic<_T, _Generator>::~FeedSynthetic()
{
    log_debug("FeedSynthetic::{}", __func__);
}

template<typename _T, typename _Generator>
void FeedSynthetic<_T, _Generator>::add_values_from_generator(const data_t& data)
{
    log_trace("FeedSynthetic::add_values_from_generator entry");

    assert (!data.empty());
    typename data_t::const_iterator iter;
    for(iter = data.begin(); iter != data.end(); ++iter)
    {
        const Data& data = *iter;
        _Generator generator(start_, end_, data.start_value, data.annual_volatility, data.annual_mean_return, this->frequency);
        rows_t rows;
        generator.generate(rows);
        assert (!rows.empty());
        base_t::add_values(data.symbol, rows);
    }
    
    log_trace("FeedSynthetic::add_values_from_generator exit");
}
