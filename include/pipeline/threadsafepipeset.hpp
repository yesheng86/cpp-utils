#pragma once
#include "pipe.hpp"
#include <set>
#include <mutex>
#include <condition_variable>

namespace util {
  namespace pipeline {

    template<class T>
    class ThreadSafePipeSet {
    public:
      ThreadSafePipeSet();
      virtual ~ThreadSafePipeSet();
      bool add(const PipePtr<T>&);
      bool remove(const PipePtr<T>&);
      bool wait_and_get_nonempty_copy(std::set<PipePtr<T>>&);
      bool try_get_nonempty_copy(std::set<PipePtr<T>>&);
      std::size_t size();
      void stop();
    private:
      std::set<PipePtr<T>> m_pipe_set;
      std::condition_variable m_condition;
      std::mutex m_mutex;
      bool m_stopped;
    };
  }
}

#include "threadsafepipeset.inl"
