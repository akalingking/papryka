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
 * @file        logger.h
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @copyright   (c) <www.sequenceresearch.com>
 */
#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/ostr.h>
#include <memory>
#include <cassert>

namespace papryka {

class Logger
{
public:
    typedef std::shared_ptr<spdlog::logger> logger_t;

    static constexpr const char* default_pattern = "%D %H:%M:%S.%e [%n][%t]: %v";

    static logger_t init(
        const std::string& name,
        const std::string& filepath = "",
        spdlog::level::level_enum logLevel = spdlog::level::debug,
        const std::string& pattern = default_pattern,
        bool async = true,
        size_t asyncQueueSize = 4096)
    {
        logger_t logger;

        if (!filepath.empty())
        {
            std::string file = filepath;
            std::string ext = ".log";
            std::size_t found = filepath.find_last_of(".");
            if (found != std::string::npos)
            {
                ext = filepath.substr(found + 1, filepath.size() - found - 1);
                file = filepath.substr(0, found);
            }

            logger = spdlog::rotating_logger_mt(name, file+ext, 1024 * 100 * 5, 5);
        }
        else
        {
            logger = spdlog::stdout_logger_mt(name);//, true);
        }

        logger->set_level(logLevel);
        spdlog::set_pattern(pattern);
        return logger;
    }
};


#ifdef _DEBUG
    static spdlog::logger* g_debug_logger = 0;
    inline spdlog::logger* get_debug_logger()
    {
        if (g_debug_logger == 0)
        {
            static std::shared_ptr<spdlog::logger> logger;
            if (!logger.get())
            {
                logger = spdlog::stdout_logger_mt("_debug_");
                logger->set_level(spdlog::level::trace);
            }
            g_debug_logger = logger.get();
        }

        assert(g_debug_logger);
        return g_debug_logger;
    }

#ifdef PAPRYKA_TRACE_ON
    template <typename...Args> static void log_trace(const char* fmt, const Args&...args)
    {
        get_debug_logger()->trace(fmt, args...);
    }
#else

    template <typename...Args> static void log_trace(const char* fmt, const Args&...args)
    {
    }
#endif
#ifdef PAPRYKA_DEBUG_ON
    template <typename...Args> static void log_debug(const char* fmt, const Args&...args)
    {
    	get_debug_logger()->debug(fmt, args...);
    }
#else

    template <typename...Args> static void log_debug(const char* fmt, const Args&...args)
    {
    }
#endif
    template <typename...Args> static void log_warn(const char* fmt, const Args&...args)
    {
        get_debug_logger()->warn(fmt, args...);
    }

    template <typename...Args> static void log_error(const char* fmt, const Args&...args)
    {
        get_debug_logger()->error(fmt, args...);
    }
#else
    template <typename...Args> static void log_trace(const char* fmt, const Args&...args)
    {
    }

    template <typename...Args> static void log_debug(const char* fmt, const Args&...args)
    {
    }

    template <typename...Args> static void log_warn(const char* fmt, const Args&...args)
    {
    }

    template <typename...Args> static void log_error(const char* fmt, const Args&...args)
    {
    }
#endif

} // namespace papryka
