/**
 * Copyright (C) 2007 Doug Judd (Zvents, Inc.)
 * 
 * This file is part of Hypertable.
 * 
 * Hypertable is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version.
 * 
 * Hypertable is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef HYPERTABLE_LOGGER_H
#define HYPERTABLE_LOGGER_H

#include <iostream>

#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#define DUMP_CORE *((int *)0) = 1;

namespace Hypertable {
  class Logger {
  public:
    static log4cpp::Category *logger;
    static void initialize(const char *name, log4cpp::Priority::Value priority=log4cpp::Priority::DEBUG);
    static void set_level(log4cpp::Priority::Value priority);
    static void set_test_mode(const char *name);
    static void suppress_line_numbers();
    static bool ms_show_line_numbers;
  };
}

#ifndef DISABLE_LOG_ALL

#ifndef DISABLE_LOG_DEBUG

#define LOG_ENTER \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->debug("(%s:%d) %s() ENTER", __FILE__, __LINE__, __func__); \
    else \
      Logger::logger->debug("%s() ENTER", __func__); \
    std::cout << std::flush; \
  }

#define LOG_EXIT \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->debug("(%s:%d) %s() EXIT", __FILE__, __LINE__, __func__); \
    else \
      Logger::logger->debug("%s() EXIT", __func__); \
    std::cout << std::flush; \
  }

#define LOG_DEBUG(msg) Logger::logger->debug("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_DEBUG(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->debug("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->debug(msg, __VA_ARGS__);  \
    std::cout << std::flush; \
   }
#else
#define LOG_ENTER
#define LOG_EXIT
#define LOG_DEBUG(msg)
#define LOG_VA_DEBUG(msg, ...)
#endif

#ifndef DISABLE_LOG_INFO
#define LOG_INFO(msg) Logger::logger->info("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_INFO(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->info("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->info(msg, __VA_ARGS__);  \
   }
#else
#define LOG_INFO(msg)
#define LOG_VA_INFO(msg, ...)
#endif

#ifndef DISABLE_LOG_NOTICE
#define LOG_NOTICE(msg) Logger::logger->notice("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_NOTICE(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->notice("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->notice(msg, __VA_ARGS__);  \
   }
#else
#define LOG_NOTICE(msg)
#define LOG_VA_NOTICE(msg, ...)
#endif

#ifndef DISABLE_LOG_WARN
#define LOG_WARN(msg) Logger::logger->warn("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_WARN(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->warn("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->warn(msg, __VA_ARGS__);  \
    std::cout << std::flush; \
   }
#else
#define LOG_WARN(msg)
#define LOG_VA_WARN(msg, ...)
#endif

#ifndef DISABLE_LOG_ERROR
#define LOG_ERROR(msg) Logger::logger->error("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_ERROR(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->error("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->error(msg, __VA_ARGS__);  \
    std::cout << std::flush; \
   }
#else
#define LOG_ERROR(msg)
#define LOG_VA_ERROR(msg, ...)
#endif

#ifndef DISABLE_LOG_CRIT
#define LOG_CRIT(msg) Logger::logger->crit("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_CRIT(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->crit("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->crit(msg, __VA_ARGS__);  \
    std::cout << std::flush; \
   }
#else
#define LOG_CRIT(msg)
#define LOG_VA_CRIT(msg, ...)
#endif

#ifndef DISABLE_LOG_ALERT
#define LOG_ALERT(msg) Logger::logger->alert("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_ALERT(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->alert("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->alert(msg, __VA_ARGS__);  \
    std::cout << std::flush; \
   }
#else
#define LOG_ALERT(msg)
#define LOG_VA_ALERT(msg, ...)
#endif

#ifndef DISABLE_LOG_EMERG
#define LOG_EMERG(msg) Logger::logger->emerg("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_EMERG(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->emerg("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->emerg(msg, __VA_ARGS__);  \
    std::cout << std::flush; \
   }
#else
#define LOG_EMERG(msg)
#define LOG_VA_EMERG(msg, ...)
#endif

#ifndef DISABLE_LOG_FATAL
#define LOG_FATAL(msg) Logger::logger->fatal("(%s:%d) " msg, __FILE__, __LINE__);
#define LOG_VA_FATAL(msg, ...) \
   { \
    if (Logger::ms_show_line_numbers) \
      Logger::logger->fatal("(%s:%d) " msg, __FILE__, __LINE__, __VA_ARGS__);  \
    else \
      Logger::logger->fatal(msg, __VA_ARGS__);  \
    std::cout << std::flush; \
   }
#else
#define LOG_FATAL(msg)
#define LOG_VA_FATAL(msg, ...)
#endif

#else // HYPERTABLE_DISABLE_LOGGING

#define LOG_DEBUG(msg)
#define LOG_VA_DEBUG(msg, ...) 
#define LOG_INFO(msg)
#define LOG_VA_INFO(msg, ...)
#define LOG_NOTICE(msg)
#define LOG_VA_NOTICE(msg, ...)
#define LOG_WARN(msg)
#define LOG_VA_WARN(msg, ...)
#define LOG_ERROR(msg)
#define LOG_VA_ERROR(msg, ...)
#define LOG_CRIT(msg)
#define LOG_VA_CRIT(msg, ...)
#define LOG_ALERT(msg)
#define LOG_VA_ALERT(msg, ...)
#define LOG_EMERG(msg)
#define LOG_VA_EMERG(msg, ...)
#define LOG_FATAL(msg)
#define LOG_VA_FATAL(msg, ...)
#define LOG_ENTER
#define LOG_EXIT

#endif // HYPERTABLE_DISABLE_LOGGING


#endif // HYPERTABLE_LOGGER_H
