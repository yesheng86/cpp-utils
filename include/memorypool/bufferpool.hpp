#pragma once
#include "base/moveonly.hpp"
#include "buffer/bufferdescriptor.hpp"

namespace util {
  namespace memorypool {
    class BufferPool : public util::base::MoveOnly {
    public:
      BufferPool();
      ~BufferPool();
      util::buffer::BufferDescriptorPtr alloc(uint32_t size);
      util::buffer::BufferDescriptorPtr alloc(uint32_t alignment, uint32_t size);
    private:
      class Impl;
      std::unique_ptr<Impl> m_p_impl;
    };
  }
}
#include "bufferpool.inl"
