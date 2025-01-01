#pragma once

#include "prerequisites.h"
#include <thread>
#include <functional>
#include <memory>
#include <atomic>

#include "tcp_connection.h"

namespace ObsCamMove {
    class TCPServer {
    public:
        using ClientHandler = std::function<void(const std::string&, std::string&)>;

        explicit TCPServer(uint16_t port);
        ~TCPServer();

        void start();
        void stop();

    private:
        asio::io_context io_context_;
        asio::ip::tcp::acceptor acceptor_;
        std::thread server_thread_;
        std::atomic_bool running_;
        std::vector<TCPConnectionPtr> connections_;

        void accept_connection();
        void remove_connection(const TCPConnectionPtr& connection);
    };
}
