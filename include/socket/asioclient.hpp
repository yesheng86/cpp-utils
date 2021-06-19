#pragma once
#include <asio.hpp>
#include <thread>
#include "onreaddelegate.hpp"

using namespace asio::ip;

namespace util {
  namespace socket {

    struct ClientConfig {
      std::string ip;
      uint16_t port;
    };

    class AsioClient {
    public:
      AsioClient();

      ~AsioClient();

      bool connect(const std::string&, const std::string&);

      bool disconnect();

      bool send_block(const BufferDescriptorPtr&);

      bool write_block(const BufferDescriptorPtr&);

      void register_delegate(const OnReadDelegatePtr&);

      void unregister_delegate();

    private:
      asio::io_context m_io_context;

      std::thread m_io_context_thread;

      tcp::resolver m_resolver;

      tcp::socket m_socket;

      OnReadDelegatePtr m_on_read_delegate;

      BufferDescriptorPtr m_current_buffer_desc;

      void start_read();

      void on_read(const std::error_code&, std::size_t);
    };
  }
}

#include "asioclient.inl"
