#pragma once

#include "prerequisites.h"
#include "message_handler.h"

namespace ObsCamMove {
    class TCPConnection;
    typedef std::shared_ptr<TCPConnection> TCPConnectionPtr;

    class TCPConnection : public std::enable_shared_from_this<TCPConnection> {
    public:
        using DisconnectCallback = std::function<void(const TCPConnectionPtr&)>;

        explicit TCPConnection(AsioTcpSocketPtr socket, DisconnectCallback disconnect_callback);

        void start();
        void close();

    private:
        AsioTcpSocketPtr socket_;
        std::array<char, 1024> buffer_;
        DisconnectCallback disconnect_callback_;
        MessageHandler message_handler_;

        void process_data();
    };
}
