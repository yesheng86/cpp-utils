#pragma once
#include "log/log.hpp"
 
inline AsioServer::AsioServer(const ServerConfig& config)
  : m_context(),
    m_acceptor(m_context, tcp::endpoint(tcp::v4(), config.port)),
    m_connections_mutex(),
    m_on_connect_callback(nullptr),
    m_on_disconnect_callback(nullptr),
    m_config(config),
    m_thread(),
    m_id(config.id),
    m_connection_id_increaser(0) {}

inline AsioServer::~AsioServer() {
  m_acceptor.close();
  if (m_thread.joinable()) {
    m_thread.join();
  }
  THREAD_SAFE_PRINT("~AsioServer");
}

inline void AsioServer::register_on_connect_callback(TOnConnectCallback callback) {
  m_on_connect_callback = callback;
}

inline void AsioServer::register_on_disconnect_callback(TOnDisconnectCallback callback) {
  m_on_disconnect_callback = callback;
}

inline void AsioServer::start() {
  m_thread = std::move(std::thread([this](){
        start_accept();
        m_context.run();
        THREAD_SAFE_PRINT("AsioServer(0x%lx) io_context exited...", m_id);
      }));
}

inline void AsioServer::stop() {
  {
    std::unique_lock lock(m_connections_mutex);
    for (AsioConnectionPtr connection : m_connections) {
      connection->close();
    }
  }
  m_context.stop();
}

inline uint64_t AsioServer::id() {
  return m_id;
}

inline void AsioServer::start_accept() {
  AsioConnectionPtr connection
    = std::make_shared<AsioConnection>(generate_connection_id());
  m_acceptor.async_accept(connection->socket(),
                          std::bind(&AsioServer::handle_accept, this,
                                    connection, std::placeholders::_1));

}

inline void AsioServer::handle_accept(const AsioConnectionPtr& conn_ptr,
                                      const std::error_code& ec) {
  if (!ec) {
    if (m_on_connect_callback) {
      m_on_connect_callback(conn_ptr);
    }
    conn_ptr->register_on_disconnect_callback
      ([this](const AsioConnectionPtr& conn_ptr){
        std::unique_lock lock(m_connections_mutex);
        if (m_on_disconnect_callback) {
          m_on_disconnect_callback(conn_ptr);
        }
        for (auto it = m_connections.begin(); it != m_connections.end(); ) {
          if ((*it).get() == conn_ptr.get()) {
            (*it)->close();
            it = m_connections.erase(it);
            THREAD_SAFE_PRINT("connection closed");
          } else {
            ++it;
          }
        }
      });
    {
      std::unique_lock lock(m_connections_mutex);
      m_connections.push_back(conn_ptr);
    }
    conn_ptr->open();
    start_accept();
  } else {
    THREAD_SAFE_PRINT("handle accept error...");
  }
}

inline uint64_t AsioServer::generate_connection_id() {
  return m_id << 32 | m_connection_id_increaser++;
}
