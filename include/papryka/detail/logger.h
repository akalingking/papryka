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
 * @date        July 9, 2016 7:51 PM
 * @copyright   (c) 2016-2027 <www.sequenceresearch.com>
 */
#pragma once
#include <spdlog/spdlog.h>
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
            const std::string& filepath="", 
            spdlog::level::level_enum logLevel=spdlog::level::debug,
            const std::string& pattern = default_pattern,
            bool async=true,
            size_t asyncQueueSize=4096)
        {   
            logger_t logger;

            if (async)
                spdlog::set_async_mode(asyncQueueSize);

            if (!filepath.empty())
            {
                std::string file = filepath;
                std::string ext = ".log";
                std::size_t found = filepath.find_last_of(".");
                if (found != std::string::npos)
                {
                    ext = filepath.substr(found+1, filepath.size() - found - 1);
                    file = filepath.substr(0, found);
                } 

                std::shared_ptr<spdlog::sinks::rotating_file_sink_mt> sink = std::make_shared<
                    spdlog::sinks::rotating_file_sink_mt>(file, ext, 1024*100*5, 5, false);

                logger = std::make_shared<spdlog::logger>(name, sink);

                spdlog::register_logger(logger);

    //            logger = spdlog::rotating_logger_mt(name, filepath, 1048576 * 5, 3);
            }
            else
            {
                logger = spdlog::stdout_logger_mt(name, true);
            }

            logger->set_level(logLevel);
            spdlog::set_pattern(pattern);
            return logger; 
        }
    };


#ifdef SPDLOG_DEBUG_ON
        static Logger::logger_t g_debug_logger;
        static const Logger::logger_t& get_debug_logger() {
            if (!g_debug_logger.get())  {
                g_debug_logger = spdlog::stdout_logger_mt("debug", true);
                g_debug_logger->set_level(spdlog::level::debug);
            }
            return g_debug_logger;
        }

        template <typename...Args>
        static void log_debug(const char* fmt, const Args&...args) {
            get_debug_logger()->debug(fmt, args...);
        }
        
        template <typename...Args>
        static void log_error(const char* fmt, const Args&...args) {
            get_debug_logger()->error(fmt, args...);
        }
#else
        template <typename...Args>
        static void log_debug(const char* fmt, const Args&...args) {}
        template <typename...Args>
        static void log_error(const char* fmt, const Args&...args) {}
#endif


#ifdef SPDLOG_TRACE_ON
        static Logger::logger_t g_trace_logger;
        static const Logger::logger_t& get_trace_logger() {
            if (!g_trace_logger.get())  {
                g_trace_logger = spdlog::stdout_logger_mt("trace", true);
                g_trace_logger->set_level(spdlog::level::trace);
            }
            return g_trace_logger;
        }

        template <typename...Args>
        static void log_trace(const char* fmt, const Args&...args) {
            get_trace_logger()->trace(fmt, args...);
        }
#else
        template <typename...Args>
        static void log_trace(const char* fmt, const Args&...args){}
#endif
    
} // namespace papryka
