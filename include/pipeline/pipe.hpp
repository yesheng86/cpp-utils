#pragma once
#include <memory>
#include "base/moveonly.hpp"
#include "waitqueue/waitqueue.hpp"
#include "memorypool/objectpool.hpp"

namespace util {
  namespace pipeline {
    template<class T>
    class Pipe : public util::base::MoveOnly {
    public:
      Pipe();
      ~Pipe();
      void push(const std::shared_ptr<T>&);
      void push(std::shared_ptr<T>&&);
      bool try_pop(std::shared_ptr<T>&);
      bool wait_and_pop(std::shared_ptr<T>&);
      void stop();
      std::shared_ptr<T> new_one();
    private:
      util::mt::WaitQueue<std::shared_ptr<T>> m_queue;
    };

    template<class T>
    using PipePtr = std::shared_ptr<Pipe<T>>;

    template<class T>
    PipePtr<T> make_pipe();
  }
}

#include "pipe.inl"
