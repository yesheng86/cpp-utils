#pragma once
#include <asio.hpp>
#include <memory>
#include <thread>
#include <atomic>
#include "buffer/bufferdescriptor.hpp"
#include "base/moveonly.hpp"
#include "onreaddelegate.hpp"


using namespace asio::ip;
using namespace util;
using namespace util::buffer;

namespace util {
  namespace socket {
    class AsioConnection;

    using AsioConnectionPtr = std::shared_ptr<AsioConnection>;

    class AsioConnection : public util::base::MoveOnly,
                           public std::enable_shared_from_this<AsioConnection> {

      typedef std::function<void (const AsioConnectionPtr&)> TOnDisconnectCallback;

    public:
      AsioConnection(uint64_t);

      virtual ~AsioConnection();

      AsioConnectionPtr get_ptr();

      tcp::socket& socket();

      void register_on_disconnect_callback(TOnDisconnectCallback);

      void register_delegate(const OnReadDelegatePtr&);

      OnReadDelegatePtr get_delegate();

      virtual void pre_open();

      virtual void post_close();

      void open();

      void close();

      void on_read_data(const std::error_code&,
                        std::size_t);

      void write_data(const BufferDescriptorPtr&);

      uint64_t id();

    private:
      ::asio::io_context m_io_context;

      tcp::socket m_socket;

      OnReadDelegatePtr m_on_read_delegate;

      TOnDisconnectCallback m_disconnect_callback;

      BufferDescriptorPtr m_current_buffer_desc;

      std::thread m_thread;

      uint64_t m_id;

      uint32_t m_buffer_alignment;

      uint32_t m_buffer_size;
    };
  }
}

#include "asioconnection.inl"
