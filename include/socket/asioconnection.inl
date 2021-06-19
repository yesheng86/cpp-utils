#pragma once
#include <functional>
#include "log/log.hpp"

using asio::ip::tcp;
using namespace util;
using namespace util::socket;

inline AsioConnection::AsioConnection
(uint64_t id)
  : m_io_context(),
    m_socket(m_io_context),
    m_on_read_delegate(nullptr),
    m_disconnect_callback(nullptr),
    m_current_buffer_desc(nullptr),
    m_thread(),
    m_id(id) {}

inline AsioConnection::~AsioConnection() {
  THREAD_SAFE_PRINT("~AsioConnection 0x%lx", m_id);
}

inline AsioConnectionPtr AsioConnection::get_ptr() {
  return shared_from_this();
}

inline tcp::socket& AsioConnection::socket() {
  return m_socket;
}

inline void AsioConnection::register_on_disconnect_callback
(TOnDisconnectCallback callback) {
  m_disconnect_callback = callback;
}

inline void AsioConnection::register_delegate
(const OnReadDelegatePtr& delegate) {
  m_on_read_delegate = delegate;
  m_on_read_delegate->
    register_write_data_function(std::bind(&AsioConnection::write_data, this,
                                           std::placeholders::_1));
}

inline OnReadDelegatePtr AsioConnection::get_delegate() {
  return m_on_read_delegate;
}

inline void AsioConnection::pre_open() {}

inline void AsioConnection::post_close() {}

inline void AsioConnection::open() {
  pre_open();
  if (m_on_read_delegate
      && !m_on_read_delegate->is_stopped()) {
    m_on_read_delegate->start();
  }
  m_thread = std::move(std::thread([this]() {
        if (m_on_read_delegate
            && !m_on_read_delegate->is_stopped()) {
          m_current_buffer_desc = m_on_read_delegate->prepare_buffer();
          if (!m_current_buffer_desc) {
            return;
          } else {
            ::asio::async_read(m_socket,
                               ::asio::buffer(m_current_buffer_desc->raw_ptr(),
                                              m_current_buffer_desc->size()),
                               std::bind(&AsioConnection::on_read_data, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
            m_io_context.run();
            THREAD_SAFE_PRINT("asio connection(0x%lx) context stopped, exit thread...", m_id);
          }
        }
      }));
  m_thread.detach();
}

inline void AsioConnection::close() {
  m_io_context.post([this] () {
      m_socket.close();
    });
  if (m_current_buffer_desc && m_on_read_delegate
      && !m_on_read_delegate->is_stopped()) {
    m_on_read_delegate->stop();
  }
  post_close();
  m_io_context.stop();
  THREAD_SAFE_PRINT("asio connection(0x%lx) context stopped", m_id);
}

inline void AsioConnection::on_read_data(const std::error_code& ec,
                                 std::size_t bytes_transferred) {
  if (!ec && m_current_buffer_desc
      && m_on_read_delegate
      && !m_on_read_delegate->is_stopped()) {
    if (!m_on_read_delegate->handle_on_read(m_current_buffer_desc, bytes_transferred)) {
      return;
    }
    m_current_buffer_desc = m_on_read_delegate->prepare_buffer();
    if (!m_current_buffer_desc) {
      return;
    } else {
      ::asio::async_read(m_socket,
                         ::asio::buffer(m_current_buffer_desc->raw_ptr(),
                                        m_current_buffer_desc->size()),
                         std::bind(&AsioConnection::on_read_data, this,
                                   std::placeholders::_1,
                                   std::placeholders::_2));
    }
  } else if (ec == ::asio::error::eof) {
    if (m_disconnect_callback) {
      THREAD_SAFE_PRINT("async disconnect connection(0x%lx)", m_id);
      m_disconnect_callback(get_ptr());
      //std::async(std::launch::async, m_disconnect_callback, get_ptr());
    }
  } else {
    THREAD_SAFE_PRINT("on read data error %s on connection(0x%x)", ec.message(), m_id);
  }
}

inline void AsioConnection::write_data(const BufferDescriptorPtr& buf_desc_ptr) {
  ::asio::write(m_socket, ::asio::buffer(buf_desc_ptr->raw_ptr(), buf_desc_ptr->size()));
  /*m_socket.async_send(::asio::buffer(data->raw_ptr(), );
  m_socket.async_send(asio::buffer(buf_desc_ptr->raw_ptr(), buf_desc_ptr->size()),
                      asio::use_future);
  */
  //THREAD_SAFE_PRINT("sent(%ld)", buf_desc_ptr->size());
}

inline uint64_t AsioConnection::id() {
  return m_id;
}

