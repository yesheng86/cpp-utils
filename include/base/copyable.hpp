#pragma once

namespace util {
  namespace base {
    class Copyable {
    public:
      Copyable(const Copyable& other) = default;
      Copyable& operator=(const Copyable& other) = default;
    };
  }
}
