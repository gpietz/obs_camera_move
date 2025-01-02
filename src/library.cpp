#include "library.h"
#include "tcp_server.h"
#include "logger.h"
#include "env_var.h"
#include <mutex>
namespace ocm = ObsCamMove;

std::unique_ptr<ObsCamMove::TCPServer> tcp_server;
std::atomic_bool obs_module_loaded(false);
std::mutex obs_module_lock;

#ifdef _WIN32
    #include <windows.h>
    HANDLE global_mutex = nullptr;
    bool create_global_mutex() {
        global_mutex = CreateMutexA(nullptr, TRUE, "Global\\OBS_Camera_Move_Mutex");
        if (global_mutex == nullptr || GetLastError() == ERROR_ALREADY_EXISTS) {
            if (global_mutex) CloseHandle(global_mutex);
            global_mutex = nullptr;
            return false;
        }
        return true;
    }
    void release_global_mutex() {
        if (global_mutex) CloseHandle(global_mutex);
    }
#else
    #include <fcntl.h>
    #include <unistd.h>
    int global_mutex_fd = -1;
    bool create_global_mutex() {
        global_mutex_fd = open("/tmp/obs_camera_move.lock", O_CREAT | O_RDWR, 0666);
        return (global_mutex_fd != -1 && flock(global_mutex_fd, LOCK_EX | LOCK_NB) == 0);
    }
    void release_global_mutex() {
        if (global_mutex_fd != -1) {
            flock(global_mutex_fd, LOCK_UN);
            close(global_mutex_fd);
        }
    }
#endif

bool obs_module_load() {
    std::lock_guard lock(obs_module_lock);

    if (!create_global_mutex()) {
        ocm::log(ocm::LogLevel::ERROR, "Failed to create global mutex. Plugin will not load.");
        return false;
    }

    if (obs_module_loaded.load()) {
        ocm::log(ocm::LogLevel::INFO, "OBS Camera Move is already loaded.");
        return true;
    }

    try {
        ocm::log(ocm::LogLevel::INFO, "**** OBS Camera Move loading ****");
        const auto tcp_port = ocm::get_env_var_int("OBS_CAMERA_MOVE_PORT", 5680);
        tcp_server = std::make_unique<ocm::TCPServer>(tcp_port);
        tcp_server->start();
        obs_module_loaded.store(true);
        ocm::log(ocm::LogLevel::INFO, "OBS Camera Move loaded successfully!");
    } catch (const std::exception &e) {
        ocm::log(ocm::LogLevel::ERROR, std::string("Exception occurred while loading plugin: ") + e.what());
        return false;
    }

    return true;
}

void obs_module_unload() {
    std::lock_guard lock(obs_module_lock);

    release_global_mutex();

    static bool already_unloaded = false;
    if (already_unloaded) {
        ocm::log(ocm::LogLevel::INFO, "obs_module_unload already processed, skipping.");
        return ;
    }
    already_unloaded = true;

    if (!obs_module_loaded.load()) {
        ocm::log(ocm::LogLevel::INFO, "OBS Camera Move is already unloaded.");
        return;
    }

    try {
        if (tcp_server) {
            ocm::log(ocm::LogLevel::INFO, "TCP Server is being stopped.");
            tcp_server->stop();
            tcp_server.reset();
            ocm::log(ocm::LogLevel::INFO, "Server stopped.");
        }
        ocm::log(ocm::LogLevel::INFO, "OBS Camera Move unloaded successfully!");
        obs_module_loaded.store(false);
    } catch (const std::exception &e) {
        ocm::log(ocm::LogLevel::ERROR, std::string("Exception occurred while unloading OBS Camera move: ") + e.what());
    }
}
