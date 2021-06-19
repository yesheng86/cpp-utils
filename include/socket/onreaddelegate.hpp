#pragma once
#include <memory>
#include <functional>
#include "buffer/bufferdescriptor.hpp"

using namespace util::buffer;

namespace util {
  namespace socket {

    class OnReadDelegate {
    public:
      OnReadDelegate();

      OnReadDelegate(uint32_t);

      OnReadDelegate(uint32_t, uint32_t);

      virtual ~OnReadDelegate() {};

      virtual BufferDescriptorPtr prepare_buffer() = 0;

      virtual bool handle_on_read(const BufferDescriptorPtr&, uint32_t) = 0;

      virtual bool start() = 0;

      virtual bool stop() = 0;

      virtual bool is_stopped() = 0;

      void set_read_buffer_size(uint32_t);

      void set_read_buffer_alignment_and_size(uint32_t, uint32_t);

      uint32_t alignment();

      uint32_t size();

      // called by asioconnection
      void register_write_data_function(const std::function<void (const BufferDescriptorPtr&)>&);

      void write_data(const BufferDescriptorPtr&);

    protected:
      std::function<void (const BufferDescriptorPtr&)> m_write_data_func;

    private:
      uint32_t m_buffer_alignment;

      uint32_t m_buffer_size;
    };
    using OnReadDelegatePtr = std::shared_ptr<OnReadDelegate>;
  }
}

#include "onreaddelegate.inl"
