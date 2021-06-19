#pragma once
#include "base/moveonly.hpp"

namespace util {
  namespace mt {
    // T is copyable and movable, std::copyable in C++20
    template<class T>
    class WaitQueue : public util::base::MoveOnly
    {
    public:
      WaitQueue();
      virtual ~WaitQueue();
      void push(const T&);
      void push(T&&);
      bool empty();
      bool try_pop(T&);
      bool wait_and_pop(T&);
      void stop_queue();
    private:
      class Impl;
      std::unique_ptr<Impl> m_p_impl;
    };
  }
}

#include "waitqueue.inl"
