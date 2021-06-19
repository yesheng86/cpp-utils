#pragma once
#include <atomic>
#include "onreaddelegate.hpp"

namespace util {
  namespace socket {

    class BufferPooledOnReadDelegate : public OnReadDelegate {
    public:
      BufferPooledOnReadDelegate();

      virtual ~BufferPooledOnReadDelegate() override;

      virtual BufferDescriptorPtr prepare_buffer() override;

      virtual bool start() override;

      virtual bool stop() override;

      virtual bool is_stopped() override;
    private:
      std::atomic_bool m_stop;
    };
    using BufferPooledOnReadDelegatePtr = std::shared_ptr<BufferPooledOnReadDelegate>;
  }
}

#include "bufferpooledonreaddelegate.inl"
