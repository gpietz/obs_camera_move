#include "tcp_connection.h"
#include "logger.h"
#include "string_utils.h"

namespace ObsCamMove {
    TCPConnection::TCPConnection(AsioTcpSocketPtr socket, DisconnectCallback disconnect_callback)
        : socket_(std::move(socket)), buffer_(), disconnect_callback_(std::move(disconnect_callback)) {
    }

    void TCPConnection::start() {
        const auto client_ep_address = socket_->remote_endpoint().address().to_string();
        log(LogLevel::DEBUG, "Starting connection for client: " + client_ep_address);
        process_data();
    }

    void TCPConnection::close() {
        if (socket_ && socket_->is_open()) {
            asio::error_code ec;
            socket_->close(ec);
            if (ec) {
                log(LogLevel::ERROR, "Error closing socket: " + ec.message());
            }
        }

        if (disconnect_callback_) {
            disconnect_callback_(shared_from_this());
        }
    }

    void TCPConnection::process_data() {
        auto self = shared_from_this(); // Prevents destruction of the current instance
        socket_->async_read_some(asio::buffer(buffer_),
            [this, self](const asio::error_code& ec, const std::size_t bytes_transferred) {
            if (!ec) {
                // Log received message
                const auto message = std::make_shared<String>(trim(String(buffer_.data(), bytes_transferred)));
                log(LogLevel::DEBUG, "Received data: " + *message);

                // +++ Parse message and send response to client +++
                std::shared_ptr<String> client_response = nullptr;

                if (const auto response = message_handler_.process_message(*message); response.has_value()) {
                    client_response = std::make_shared<String>(response.value());
                } else {
                    client_response = std::make_shared<String>("No response received for message: " + *message);
                }

                async_write(*socket_, asio::buffer(*client_response),
                    [this, self, client_response](const asio::error_code& ec2, const std::size_t) {
                    if (ec2) {
                        log(LogLevel::ERROR, "Error writing to socket: " + ec2.message());
                    } else {
                        log(LogLevel::DEBUG, "Data successful send: " + *client_response);
                    }
                });

                // Read more data
                process_data();
            } else if (ec == asio::error::eof) {
                log(LogLevel::INFO, "Client disconnected.");
                close();
            } else {
                log(LogLevel::ERROR, "Error reading data: " + ec.message());
                close();
            }
        });
    }
}
