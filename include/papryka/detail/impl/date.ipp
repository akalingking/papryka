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
 * @file        date.ipp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 2, 2016 8:41 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
namespace {
    time_t get_utc_offset()
    {
        std::time_t current_time;
        std::time(&current_time);
        struct std::tm timeinfo;
        timeinfo = *std::localtime(&current_time);
        return timeinfo.tm_gmtoff;
    }
    
    bool to_tm(struct tm& tm, const datetime_t& datetime, const char* tz=nullptr)
    {
        bool ret = true;
        time_t t;
        if (tz == nullptr || !strcmp(tz, "UTC"))
        {
            t = Clock::to_time_t(datetime);
            tm = *std::gmtime(&t);
        }
        else if (!strcmp(tz, "localtime"))
        {
            t = Clock::to_time_t(datetime);
            tm = *std::localtime(&t);
        }
        else
        {
            // due to setenv, this is slow!
            char* old_tz = std::getenv("TZ");
            setenv("TZ", tz, 1);
            tzset();
            
            t = Clock::to_time_t(datetime);
            time_t offset = get_utc_offset();
            char * tz = getenv("TZ");
            
            if (offset != 0 && tz != nullptr)
            {
                //Log4_DEBUG("Timezone \"%s\" offset=%ld", tz, offset);
                tm = *std::localtime(&t);
            }
            else
            {
                log_error("invalid timezone \"{}\" offset={ld}", tz, offset);
                ret = false;
            }
            
            if (old_tz != nullptr)
                setenv("TZ", old_tz, 1);
            else
                unsetenv("TZ");
            tzset();
        }    
        return ret;
    }   
} // namespace

/**
 * Dateformat reference: http://pubs.opengroup.org/onlinepubs/009695399/functions/strptime.html
 * assumes input date value is based on gm
 */
time_t to_time_t(const char* date, const char* dateFormat)
{
    static char buff[30];
    memset(&buff, '\0', 30);
    struct tm tm_;
    memset(&tm_, '\0', sizeof(struct tm));
    strptime(date, dateFormat, &tm_);
    // Tell mktime to figure out the daylight saving time
    tm_.tm_isdst = -1;
    time_t t_ = mktime(&tm_); 
    // mktime localtime, get offset to get gmtime
    time_t offset = get_utc_offset();
    //std::cout << offset << std::endl;
    t_ += offset;
    return t_;
}
 
datetime_t to_datetime(const char* date, const char* dateFormat)
{
    return Clock::from_time_t(to_time_t(date, dateFormat));
}

date_t to_date(const char* date, const char* dateFormat)
{
     datetime_t datetime = Clock::from_time_t(to_time_t(date, dateFormat));
     return to_date(datetime);
}

datetime_t to_datetime(const date_t& date)
{
    return std::chrono::time_point_cast<Nanoseconds>(date);
}

date_t to_date(const datetime_t& datetime)
{
    return std::chrono::time_point_cast<Days>(datetime);
}

const char* to_str(const date_t& date, const char* dateFormat)
{   
    static char szdate[20];
    memset(szdate, '\0', 20);
    time_t t_ = std::chrono::duration_cast<Seconds>(date.time_since_epoch()).count();
    struct tm tm_ = *std::gmtime(&t_);
    std::strftime(szdate, 20, dateFormat, &tm_);
    return szdate;
}

const char* to_str(const datetime_t& datetime, const char* dateFormat, const char* tz)
{   
    static const size_t date_max_len = 25;
    static char szdate[date_max_len];
    memset(szdate, '\0', 20);
    
    time_t t; 

    struct tm tm_;
    if (tz==nullptr || !strcmp(tz, "UTC"))
    {
        t = std::chrono::duration_cast<Seconds>(datetime.time_since_epoch()).count();
        tm_ = *std::gmtime(&t);
        std::strftime(szdate, 20, dateFormat, &tm_);
    }
    else if (!strcmp(tz, "LOCALTIME"))
    {
        t = std::chrono::duration_cast<Seconds>(datetime.time_since_epoch()).count();
        tm_ = *std::localtime(&t);
        std::strftime(szdate, 20, dateFormat, &tm_);
    }
    else
    {
        // Find offset, this will be really slow
        // https://www.vmware.com/support/developer/vc-sdk/visdk400pubs/ReferenceGuide/timezone.html
        // https://users.pja.edu.pl/~jms/qnx/help/watcom/clibref/global_data.html
        // NOTE: this is working only on specific uniz TZ format such as: EST5EDT, PST8PDT
        t = std::chrono::duration_cast<Seconds>(datetime.time_since_epoch()).count();
        
        char* old_tz = std::getenv("TZ");
        setenv("TZ", tz, 1);
        tzset();
        
        time_t offset = get_utc_offset();
        if (offset != 0 && getenv("TZ") != nullptr)
        {
            tm_ = *std::localtime(&t);
            std::strftime(szdate, 20, dateFormat, &tm_);
        }
        else
        {
            log_error("timezone \"{}\", invalid offset {ld}", getenv("TZ"), offset);
        }
        
        if (old_tz != nullptr)
            setenv("TZ", old_tz, 1);
        else
            unsetenv("TZ");
        
        tzset();
    }
    
    // make ms default for datetime printing
    //if (std::string(dateFormat).find("%S.000") != std::string::npos)
    if (std::string(dateFormat).find("%S") != std::string::npos)
    {
        // include millisecond timestamp
        Milliseconds ms = std::chrono::duration_cast<Milliseconds>(datetime.time_since_epoch());
        size_t fractional_seconds = ms.count() % 1000;
        std::string s_ms = std::to_string(fractional_seconds);
        s_ms = "." + s_ms;
        assert (strlen(szdate)+ s_ms.size() <= date_max_len);
        strcat(szdate, s_ms.c_str());
    }
    
    return szdate;
}

datetime_t to_tz(const datetime_t& datetime, const char* tz)
{
    // NOTE: this is not thread safe.
    // Due to setenv, this is slow!
    datetime_t ret = nulldate;
    //char old_tz[20] = {'\0'};
    char* old_tz = std::getenv("TZ");
    setenv("TZ", tz, 1);
    tzset();

    time_t offset = get_utc_offset();
    if (offset != 0 && getenv("TZ") != nullptr)
    {
        time_t t = Clock::to_time_t(datetime);
        std::tm tm = *std::localtime(&t);
        t = std::mktime(&tm);
        ret = Clock::from_time_t(t);
    }
    else
    {
        log_error("timezone \"{}\", invalid offset {ld}", getenv("TZ"), offset);
    }

    if (old_tz != nullptr)
        setenv("TZ", old_tz, 1);
    else
        unsetenv("TZ");
    tzset();

    return ret;
}

int get_year(const datetime_t& datetime)
{
    std::tm tm = {0};
    return (to_tm(tm, datetime) ? (tm.tm_year + 1900) : -1);
}

int get_month(const datetime_t& datetime)
{
    int ret = -1;
    std::tm tm = {0};
    if (to_tm(tm, datetime))
        ret = tm.tm_mon + 1; // 0-11, from Jan
    return ret;   
}

int get_day(const datetime_t& datetime, const char* tz)
{
    int ret = -1;
    std::tm tm = {0};
    if (to_tm(tm, datetime, tz))
        ret = tm.tm_mday; // 0-6, from Sunday
    return ret;
}

int get_hour(const datetime_t& datetime, const char* tz)
{
    int ret = -1;
    std::tm tm = {0};
    if (to_tm(tm, datetime, tz))
        ret = tm.tm_hour; // 0-23
    else
        assert (false);
    return ret;
}

int get_minute(const datetime_t& datetime)
{
    int ret = -1;
    std::tm tm = {0};
    if (to_tm(tm, datetime))
        ret = tm.tm_min; // 0-59
    return ret;
}

int get_second(const datetime_t& datetime)
{
    int ret = 0;
    std::tm tm = {0};
    if (to_tm(tm, datetime))
        ret = tm.tm_sec;//0-59
    return ret;
}

unsigned long get_millisecond(const datetime_t& datetime)
{
    const std::chrono::duration<double> tse = datetime.time_since_epoch();
    Seconds::rep ms = std::chrono::duration_cast<Milliseconds>(tse).count() % 1000;
    return ms;
}

bool is_weekday(const datetime_t& datetime, const char* tz)
{
    bool ret = false;
    std::tm tm;
    if (to_tm(tm, datetime, tz))
        ret = (tm.tm_wday != 0 && tm.tm_wday != 6);
    else
        assert (false);
    return ret;
}


template <class Int>
constexpr Int days_from_civil(Int y, unsigned m, unsigned d) noexcept
{
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
             "This algorithm has not been ported to a 16 bit unsigned integer");
    static_assert(std::numeric_limits<Int>::digits >= 20,
             "This algorithm has not been ported to a 16 bit signed integer");
    y -= m <= 2;
    const Int era = (y >= 0 ? y : y-399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);      // [0, 399]
    const unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
    const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
    return era * 146097 + static_cast<Int>(doe) - 719468;
}

template <class Int>
constexpr std::tuple<Int, unsigned, unsigned>
civil_from_days(Int z) noexcept
{
    static_assert(std::numeric_limits<unsigned>::digits >= 18,
             "This algorithm has not been ported to a 16 bit unsigned integer");
    static_assert(std::numeric_limits<Int>::digits >= 20,
             "This algorithm has not been ported to a 16 bit signed integer");
    z += 719468;
    const Int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(z - era * 146097);          // [0, 146096]
    const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
    const Int y = static_cast<Int>(yoe) + era * 400;
    const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
    const unsigned mp = (5*doy + 2)/153;                                   // [0, 11]
    const unsigned d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
    const unsigned m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
    return std::tuple<Int, unsigned, unsigned>(y + (m <= 2), m, d);
}

template <class Int>
constexpr unsigned
weekday_from_days(Int z) noexcept
{
    return static_cast<unsigned>(z >= -4 ? (z+4) % 7 : (z+5) % 7 + 6);
}

template <class To, class Rep, class Period>
To round_down(const std::chrono::duration<Rep, Period>& d)
{
    To t = std::chrono::duration_cast<To>(d);
    if (t > d)
        --t;
    return t;
}

template <class Duration>
std::tm utc_tm(std::chrono::time_point<std::chrono::system_clock, Duration> tp)
{
    using namespace std;
    using namespace std::chrono;
    typedef duration<int, ratio_multiply<hours::period, ratio<24>>> days;
    // t is time duration since 1970-01-01
    Duration t = tp.time_since_epoch();
    // d is days since 1970-01-01
    days d = round_down<std::chrono::days>(t);
    // t is now time duration since midnight of day d
    t -= d;
    // break d down into year/month/day
    int year;
    unsigned month;
    unsigned day;
    std::tie(year, month, day) = civil_from_days(d.count());
    // start filling in the tm with calendar info
    std::tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_wday = weekday_from_days(d.count());
    tm.tm_yday = d.count() - days_from_civil(year, 1, 1);
    // Fill in the time
    tm.tm_hour = duration_cast<hours>(t).count();
    t -= hours(tm.tm_hour);
    tm.tm_min = duration_cast<minutes>(t).count();
    t -= minutes(tm.tm_min);
    tm.tm_sec = duration_cast<seconds>(t).count();
    return tm;
}

qdatetime_t to_qdate(const datetime_t datetime)
{
    time_t tt = to_time_t(to_str(datetime), s_datetime_format);
    struct tm local_tm= *std::localtime(&tt);
    return qdatetime_t(local_tm.tm_mday, (ql::Month)((int)local_tm.tm_mon+1), local_tm.tm_year+1900);
}
