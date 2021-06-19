#pragma once
namespace util {
  namespace base {
    class NotCopyableOrMovable {
    public:
      NotCopyableOrMovable() = default;
      NotCopyableOrMovable(const NotCopyableOrMovable&) = delete;
      NotCopyableOrMovable& operator=(const NotCopyableOrMovable&) = delete;
      NotCopyableOrMovable(NotCopyableOrMovable&&) = delete;
      NotCopyableOrMovable& operator=(NotCopyableOrMovable&&) = delete;
    };
  }
}
