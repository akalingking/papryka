
FeedCsv<Bar>::FeedCsv(const std::string& dateColumn, const std::string& dateFormat, const char delimiter, size_t maxLen, Frequency frequency) :
        base_t(maxLen, frequency), delimiter(delimiter), date_column(dateColumn), date_format(dateFormat)        
{
}

void FeedCsv<Bar>::add_values_from_csv(const std::string& symbol, const std::string& filepath)
{
    log_trace("FeedCsv<Bar>::{} reading file:{} dateformat:{}", __func__, filepath, date_format);
    
    std::ifstream file(filepath);
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
            for (int i=1; i<header_size; ++i)
            {
                std::string name = headers[i];
                double val = 0.0;
                if (i < line_values.size())
                {
                    std::string strVal = line_values[i];
                    util::trim(strVal);
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
            bar.closeadj =(row_data[5];//adjClose
            
            log_debug("FeedCsv<Bar>::{0:} o:{1:.3f} h:{2:.3f} l:{3:.3f} c:{4:.3f} v:{5:.3f} a:{6:.3f}", __func__, bar.open, bar.high, bar.low, bar.close, bar.volume, bar.closeadj);
            
            if (row_filter_)
            {
                if (row_filter_->include_row(date, bar))
                    rows.push_back(bar);
            }
            else
            {
                rows.push_back(bar);
            }
        }
        
        file.close();
        
        if (!rows.empty()) 
        {
            std::sort(rows.begin(), rows.end(), [](const row_t& lhs, const row_t& rhs)->bool{ return std::get<0>(lhs) < std::get<0>(rhs);});
            this->add_values(name, rows);
        }
    }
}
