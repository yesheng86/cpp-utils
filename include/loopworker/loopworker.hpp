#pragma once
#include <thread>
#include <atomic>
#include <memory>
#include "base/moveonly.hpp"

namespace util {
  namespace mt {
    class LoopWorker : public util::base::MoveOnly {
    public:
      LoopWorker();
      virtual ~LoopWorker();
      virtual void start();
      virtual void stop();
      virtual bool do_work() = 0;
      virtual bool pre_work();
      virtual bool post_work();
    private:
      std::atomic_bool m_stopped;
    };
  }
}

#include "loopworker.inl"
