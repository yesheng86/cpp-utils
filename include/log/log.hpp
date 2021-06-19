#pragma once
#include <string>
#include <cstdarg>
#include <cstdio>
#include <mutex>
#include "singleton/singleton.hpp"

#define LOG_BUF_SZ 2048

namespace util {
  namespace log {

    class ThreadSafePrint {
    public:
      ThreadSafePrint() = default;

      virtual ~ThreadSafePrint()  = default;

      void print(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        {
          std::scoped_lock lock(m_mutex);
          vsnprintf(m_buf, sizeof(m_buf), fmt, args);
        }
        va_end(args);
        printf("%s\n", m_buf);
      }

      void log(const char *func, const char *msg, const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        {
          std::scoped_lock lock(m_mutex);
          vsnprintf(m_buf, sizeof(m_buf), fmt, args);
        }
        va_end(args);
        printf("[%s] %s %s\n", func, msg, m_buf);
      }

    private:
      std::mutex m_mutex;
      char m_buf[LOG_BUF_SZ];
    };

  }
}

#define THREAD_SAFE_PRINT util::Singleton<util::log::ThreadSafePrint>::instance().print
