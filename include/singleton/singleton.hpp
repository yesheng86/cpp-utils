#pragma once
#include "base/notcopyableormovable.hpp"

namespace util {
  template <typename T>
  class Singleton : public util::base::NotCopyableOrMovable {
  public:
    static T& instance(){
      static T instance;
      return instance;
    }
  protected:
    Singleton() = default;
    ~Singleton() = default;
  };
}
