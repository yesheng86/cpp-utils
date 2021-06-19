#pragma once
namespace util {
  namespace base {
    class MoveOnly {
    public:
      MoveOnly() = default;
      MoveOnly(MoveOnly&& other) = default;
      MoveOnly& operator=(MoveOnly&& other) = default;
      MoveOnly(const MoveOnly&) = delete;
      MoveOnly& operator=(const MoveOnly&) = delete;
    };
  }
}
