#pragma once
#include <asio.hpp>
#include <thread>
#include <mutex>
#include "onreaddelegate.hpp"
#include "asioconnection.hpp"

using namespace asio::ip;

namespace util {
  namespace socket {

    struct ServerConfig {
      uint64_t id;
      uint16_t port;
    };

    class AsioServer {
      typedef std::function<void (const AsioConnectionPtr&)> TOnConnectCallback;

      typedef std::function<void (const AsioConnectionPtr&)> TOnDisconnectCallback;
    public:
      AsioServer(const ServerConfig&);

      ~AsioServer();

      void register_on_connect_callback(TOnConnectCallback);

      void register_on_disconnect_callback(TOnDisconnectCallback);

      void start();

      void stop();

      uint64_t id();

    private:
      void start_accept();

      void handle_accept(const AsioConnectionPtr&, const std::error_code&);

      uint64_t generate_connection_id();

      ::asio::io_context m_context;

      tcp::acceptor m_acceptor;

      std::vector<AsioConnectionPtr> m_connections;

      std::mutex m_connections_mutex;

      TOnConnectCallback m_on_connect_callback;

      TOnDisconnectCallback m_on_disconnect_callback;

      ServerConfig m_config;

      std::thread m_thread;

      uint64_t m_id;

      uint64_t m_connection_id_increaser;
    };
  }
}

#include "asioserver.inl"
