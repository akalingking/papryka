template<typename _T>
FeedCsv<_T>::FeedCsv(const std::string& dateColumn, const std::string& dateFormat, const char delimiter, Frequency frequency, size_t maxLen) :
        base_t(frequency, maxLen), delimiter(delimiter), date_column(dateColumn), date_format(dateFormat)        
{
}

template<typename _T>
void FeedCsv<_T>::add_values_from_csv(const std::string& symbol, const std::string& filepath)
{
    log_trace("FeedCsv<_T>::{} reading file:{} dateformat:{}", __func__, filepath, date_format);
    
    std::ifstream file(filepath.c_str());
    if (!file.is_open())
    {
        log_error("FeedCsv<_T>::{} failed to open {}", __func__, filepath);
        assert (false);
    }
    else
    {
        // read header
        std::string line;
        std::getline(file, line);
        //std::cout << "Line: " << line << std::endl;
        
        std::vector<std::string> headers;
        string::tokenize(line, headers, ",");
        size_t header_size = headers.size(); 
        
        // read data
        rows_t rows; 
        while(std::getline(file, line))
        {
            //log_debug("FeedCsv<_T>::{} line='{}'", __func__, line);
            
            if (line.empty())
            {
                log_warn("FeedCsv<_T>::{} empty line", __func__);
                continue;
            }
            std::vector<std::string> line_values;
            string::tokenize(line, line_values, ",");
            assert (!line_values.empty());
            
            datetime_t date = to_datetime(line_values[0].c_str(), date_format.c_str());
            double value = 0.0;
            if (header_size > 2)
                value = std::stod(line_values[4]);
            else
                value = std::stod(line_values[1]);
            
            _T type_value = (_T)value; 
            
            log_trace("FeedCsv<_T>::{} date={} value={}", __func__, to_str(date), value);
            
            if (base_t::row_filter_)
            {
                if (base_t::row_filter_->include_row(date, type_value))
                    rows.push_back(row_t(date, type_value));
            }
            else
            {
                rows.push_back(row_t(date, type_value));
            }
        }
        
        file.close();
        
        if (!rows.empty()) 
        {
            std::sort(rows.begin(), rows.end(), [](const row_t& lhs, const row_t& rhs)->bool{ return std::get<0>(lhs) < std::get<0>(rhs);});
            log_debug("FeedCsv<_T>::{} add {} rows", __func__, rows.size());
            base_t::add_values(symbol, rows);
        }
    }
}

FeedCsv<Bar>::FeedCsv(const std::string& dateColumn, const std::string& dateFormat, const char delimiter, Frequency frequency, size_t maxLen) :
        base_t(frequency, maxLen), delimiter(delimiter), date_column(dateColumn), date_format(dateFormat)        
{
}

void FeedCsv<Bar>::add_values_from_csv(const std::string& symbol, const std::string& filepath)
{
    log_trace("FeedCsv<Bar>::{} reading file:{} dateformat:{}", __func__, filepath, date_format);
    
    std::ifstream file(filepath.c_str());
    if (!file.is_open())
    {
        log_error("FeedCsv<Bar>::{} failed to open {}", __func__, filepath);
        assert (false);
    }
    else
    {
        // read header
        std::string line;
        std::getline(file, line);
        //std::cout << "Line: " << line << std::endl;
        
        std::vector<std::string> headers;
        string::tokenize(line, headers, ",");
        size_t header_size = headers.size(); 
        
        // read data
        base_t::rows_t rows; 
        while(std::getline(file, line))
        {
            if (line.empty())
            {
                log_warn("FeedCsv<_T>::{} empty line", __func__);
                continue;
            }
            std::vector<std::string> line_values;
            string::tokenize(line, line_values, ",");
            assert (!line_values.empty());
            
            datetime_t date = to_datetime(line_values[0].c_str(), date_format.c_str());
            assert (date != nulldate);
            std::vector<double> row_data;
            for (size_t i=1; i < header_size; ++i)
            {
                std::string name = headers[i];
                double val = 0.0;
                if (i < line_values.size())
                {
                    std::string strVal = line_values[i];
                    string::trim(strVal);
                    if (!strVal.empty())
                        val = std::stod(strVal);
                    row_data.push_back(val);
                }    
            }
            
            Bar bar;
            bar.open = row_data[0];
            bar.high = row_data[1];
            bar.low = row_data[2];//low
            bar.close = row_data[3];//close
            bar.volume = row_data[4];//volume
            bar.closeadj = row_data[5];//adjClose
            
            log_trace("FeedCsv<Bar>::{0:} {1:} o:{2:.3f} h:{3:.3f} l:{4:.3f} c:{5:.3f} a:{6:.3f} v:{7:.3f}", __func__, 
                    to_str(date), bar.open, bar.high, bar.low, bar.close, bar.closeadj, bar.volume);
            
            if (row_filter_)
            {
                if (row_filter_->include_row(date, bar))
                    rows.push_back(row_t(date, bar));
            }
            else
            {
                rows.push_back(row_t(date, bar));
            }
        }
        
        file.close();
        
        if (!rows.empty()) 
        {
            std::sort(rows.begin(), rows.end(), [](const row_t& lhs, const row_t& rhs)->bool{ return std::get<0>(lhs) < std::get<0>(rhs);});
            add_values(symbol, rows);
        }
    }
}
