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
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */ 
FeedGenerator_::FeedGenerator_(const datetime_t& start, const datetime_t& end, Frequency frequency) :
    start(start),
    end(end),
    frequency(frequency),
    dist(0.1, 1.0)
{
    rng.seed(50000);
    log_trace("FeedGenerator_ created");
}

/*GenericGenerator::GenericGenerator(const datetime_t& start, const datetime_t& end, Frequency frequency) :
    start(start),
    end(end),
    frequency(frequency),
    dist(0.1, 1.0)
{
    rng.seed(50000);
    log_trace("GenericGenerator created");
}

size_t GenericGenerator::generate(rows_t& rows)
{
    log_trace("GenericGenerator::generate");
    
    datetime_t date = start;
    ql::Calendar cal = ql::UnitedStates();
    size_t counter = 0;
    while (date <= end && counter++ < max_len)
    {
        if (cal.isBusinessDay(to_qdate(date)))
        {
            float value = (float)dist(rng);
            log_debug("GenericGenerator::generate [{0:}] date={1:} value={2:0.3f}", counter, to_str(date), value);
            row_t row = row_t(date, value);
            rows.push_back(row);
        }
        
        date = get_next_timepoint(date, frequency);
    }
    return counter;
}*/

template<typename _T, typename _Generator>
SyntheticFeed<_T, _Generator>::SyntheticFeed(datetime_t start, const datetime_t& end, Frequency frequency) :
    base_t(1024, frequency),
    start_(start),
    end_(end),
    generator_(start, end, frequency)
{
    log_trace("SyntheticFeed created");
}

template<typename _T, typename _Generator>
void SyntheticFeed<_T, _Generator>::add_values_from_generator(const std::vector<std::string>& instruments)
{
    log_trace("SyntheticFeed::add_values_from_generator entry");

    assert (!instruments.empty());
    std::vector<std::string>::const_iterator iter;
    for(iter = instruments.begin(); iter != instruments.end(); ++iter)
    {
        rows_t rows;
        generator_.generate(rows);
        assert (!rows.empty());
        base_t::add_values(*iter, rows);
    }
    
    log_trace("SyntheticFeed::add_values_from_generator exit");
}
