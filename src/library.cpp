#include "library.h"
#include "tcp_server.h"
#include "logger.h"
#include "env_var.h"
namespace ocm = ObsCamMove;

std::unique_ptr<ObsCamMove::TCPServer> tcp_server;

bool obs_module_load() {
    try {
        ocm::log(ocm::LogLevel::INFO, "OBS Camera Move loaded!");
        const auto tcp_port = ObsCamMove::get_env_var_int("OBS_CAMERA_MOVE_PORT", 5680);
        tcp_server = std::make_unique<ObsCamMove::TCPServer>(tcp_port);
        tcp_server->start();
    } catch (const std::exception &e) {
        ocm::log(ocm::LogLevel::ERROR, std::string("Exception occurred while loading plugin: ") + e.what());
    }

    return true;
}

void obs_module_unload() {

    try {
        if (tcp_server) {
            tcp_server->stop();
            tcp_server.reset();
        }
    } catch (const std::exception &e) {
        ocm::log(ocm::LogLevel::ERROR, std::string("Exception occurred while unloading plugin: ") + e.what());
    }

    ocm::log(ocm::LogLevel::INFO, "OBS Camera Move unloaded!");
}
