#pragma once
#include "base/moveonly.hpp"

namespace util {
  namespace memorypool {

    template <class T>
    class ObjectPool : public util::base::MoveOnly {
    public:
      ObjectPool();
      ~ObjectPool();
      std::shared_ptr<T> new_one();
    private:
      template <class U>
      class Impl;
      std::unique_ptr<Impl<T>> m_p_impl;
    };
  }
}

#include "objectpool.inl"
