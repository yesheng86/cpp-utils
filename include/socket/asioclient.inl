#pragma once
#include "log/log.hpp"

using namespace util::socket;
using namespace util::log;

inline AsioClient::AsioClient()
  : m_io_context(),
    m_io_context_thread(),
    m_resolver(m_io_context),
    m_socket(m_io_context, tcp::v4()) {
}

inline AsioClient::~AsioClient() {
  THREAD_SAFE_PRINT("~AsioClient");
  m_io_context_thread.join();
}

inline bool AsioClient::connect(const std::string& ip, const std::string& port) {
  auto work = asio::require(m_io_context.get_executor(),
                            asio::execution::outstanding_work.tracked);
  asio::error_code error;
  tcp::resolver::results_type endpoints = m_resolver.resolve(tcp::v4(), ip, port, error);
  if (error) {
    THREAD_SAFE_PRINT("resolve end point failed...");
    return false;
  }
#if 0
  auto ep = *endpoints.begin();
  m_socket.connect(ep, error);
  if (error) {
    THREAD_SAFE_PRINT("socket connect failed...");
    return false;
  }
  if (m_on_read_delegate) {
    start_read();
  }
#else
  asio::async_connect(m_socket, endpoints,
                      [this](std::error_code ec, tcp::endpoint)
                      {
                        THREAD_SAFE_PRINT("socket connect ec:%x", ec);
                        if (!ec)
                          {
                            if (m_on_read_delegate) {
                              start_read();
                            }
                          }
                      });
#endif
  m_io_context_thread = std::thread([this]() { m_io_context.run(); });
  return true;
}

inline bool AsioClient::disconnect() {
  m_io_context.stop();
  THREAD_SAFE_PRINT("disconnected...");
  return true;
}

inline bool AsioClient::send_block(const BufferDescriptorPtr& buf_desc_ptr) {
  m_socket.async_send(asio::buffer(buf_desc_ptr->raw_ptr(), buf_desc_ptr->size()),
                      asio::use_future);
  return true;
}

inline bool AsioClient::write_block(const BufferDescriptorPtr& buf_desc_ptr) {
  ::asio::write(m_socket, ::asio::buffer(buf_desc_ptr->raw_ptr(), buf_desc_ptr->size()));
  return true;
}

inline void AsioClient::register_delegate(const OnReadDelegatePtr& delegate) {
  m_on_read_delegate = delegate;
}

inline void AsioClient::unregister_delegate() {
  m_on_read_delegate = nullptr;
}

inline void AsioClient::start_read() {
  THREAD_SAFE_PRINT("AsioClient::start_read");
  m_current_buffer_desc = m_on_read_delegate->prepare_buffer();
  if (!m_current_buffer_desc) {
    THREAD_SAFE_PRINT("AsioClient::start_read null buffer desc");
    return;
  }
  THREAD_SAFE_PRINT("AsioClient::start_read start async read %d", m_current_buffer_desc->size());
  asio::async_read(m_socket,
                   asio::buffer(m_current_buffer_desc->raw_ptr(),
                                m_current_buffer_desc->size()),
                   std::bind(&AsioClient::on_read, this,
                             std::placeholders::_1,
                             std::placeholders::_2));
}

inline void AsioClient::on_read(const std::error_code& ec,
                                std::size_t bytes_transferred) {
  THREAD_SAFE_PRINT("AsioClient::on_read %d transferred", bytes_transferred);
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
                         std::bind(&AsioClient::on_read, this,
                                   std::placeholders::_1,
                                   std::placeholders::_2));
    }
  } else if (ec == ::asio::error::eof) {
    THREAD_SAFE_PRINT("asioclient receive eof");
  } else {
    THREAD_SAFE_PRINT("on read data error %s on connection(0x%x)", ec.message()    );
  }
}
