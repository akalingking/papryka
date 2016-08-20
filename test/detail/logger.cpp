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
 * @file        logger.cpp
 * @author      Ariel Kalingking  <akalingking@sequenceresearch.com>
 * @date        July 2, 2016 8:02 PM
 * @copyright   (c) 2016-2026 <www.sequenceresearch.com>
 */
#include <gtest/gtest.h>
#include <papryka/papryka.h>

using namespace papryka;

TEST(Detail, logger)
{
#if 0
    Logger::logger_t logger = Logger::init("test", "./test.log");
    logger->debug("This is a test");
    logger->debug("{}", "this is a log test");
    

    Logger::logger_t console = Logger::init("console");    
    console->debug("test from console");
    console->debug("{}", "test from console");
    console->debug("{0:0.3f}", 1.23456);
    console->trace("console trace");
    spdlog::get("console")->debug("test from console using spdlog::get");

#ifndef PAPRYKA_TRACE_ON     
    log_trace("this is a log {} will not print", "trace");
#else
    log_trace("this is a log {} will print", "trace");
#endif
#ifndef PAPRYKA_DEBUG_ON    
    log_debug("this is a log {} will not print", "debug");
#else
    log_debug("this is a log {} will print", "debug");
#endif
#endif
}
