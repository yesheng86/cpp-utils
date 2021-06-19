#pragma once
#include <memory>
#include "base/notcopyableormovable.hpp"

using namespace util::base;

namespace util {
  namespace buffer {

    class BufferDescriptor : public NotCopyableOrMovable {
    public:
      BufferDescriptor(void* ptr, uint32_t size)
        : BufferDescriptor(ptr, 0, size) {}

      BufferDescriptor(void* ptr, uint32_t alignment, uint32_t size)
        : NotCopyableOrMovable(),
          m_raw_ptr(ptr), m_alignment(alignment), m_size(size) {}

      ~BufferDescriptor() = default;

      void* raw_ptr() { return m_raw_ptr; }

      uint32_t alignment() { return m_alignment; }

      uint32_t size() { return m_size; }

    private:
      void* m_raw_ptr;

      uint32_t m_alignment;

      uint32_t m_size;
    };

    using BufferDescriptorPtr = std::shared_ptr<BufferDescriptor>;
  }
}
