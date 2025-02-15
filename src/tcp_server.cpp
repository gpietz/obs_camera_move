#define _WIN32_WINNT 0x0A00 // Windows 10/11
#include "tcp_server.h"
#include "logger.h"
#include "tcp_connection.h"
#include <mutex>

static std::mutex server_lock;

namespace ObsCamMove {
    TCPServer::TCPServer(const uint16_t port)
        : acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
          running_(false) {}

    TCPServer::~TCPServer() {
        stop();
    }

    void TCPServer::start() {
        running_.store(true);

        // Start accepting connections
        accept_connection();

        // Start the io_context thread
        server_thread_ = std::thread([this] {
            try {
                io_context_.run();
            } catch (const std::exception& e) {
                log(LogLevel::ERROR, std::string("Server error: ") + e.what());
            }
        });

        log(LogLevel::INFO, std::string("Server started on port ") + std::to_string(acceptor_.local_endpoint().port()));
    }

    void TCPServer::stop() {
        std::lock_guard lock(server_lock);

        if (bool expected = true; running_.compare_exchange_strong(expected, false)) {
            io_context_.stop();
            if (server_thread_.joinable()) {
                server_thread_.join();
            }
            log(LogLevel::INFO, std::string("Server stopped."));
        } else {
            log(LogLevel::INFO, "Stop called but server was already stopped.");
        }
    }

    void TCPServer::accept_connection() {
        // Create a new socket for the incoming connection
        auto socket = std::make_shared<asio::ip::tcp::socket>(io_context_);
        acceptor_.async_accept(*socket, [this, socket](const asio::error_code& ec) {
            if (!ec) {
                // Check whether the connection is coming from localhost
                const auto remote_endpoint = socket->remote_endpoint();
                const auto remote_address = remote_endpoint.address().to_string();

                if (remote_address == "127.0.0.1") {
                    std::ostringstream oss;
                    oss << "Client Connection from " << socket->remote_endpoint();
                    log(LogLevel::INFO, oss.str());

                    const auto connection = std::make_shared<TCPConnection>(socket,
                        [this](const TCPConnectionPtr& conn) {
                        remove_connection(conn);
                    });
                    connections_.push_back(connection);
                    connection->start();
                } else {
                    log(LogLevel::WARN, "Rejected connection from: " + remote_address);
                    socket->close();
                }
            } else {
                log(LogLevel::ERROR, std::string("Accepted error: ") + ec.message());
            }

            // Make sure that the server is still running
            if (running_.load()) {
                accept_connection();
            }
        });
    }

    void TCPServer::remove_connection(const TCPConnectionPtr& connection) {
        log(LogLevel::INFO, "Removing connection");
        std::erase(connections_, connection);
    }
}
