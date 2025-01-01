#include "camera_controller.h"
#include "logger.h"
#include <obs.h>
#include <obs-frontend-api.h>
#include "string_utils.h"

namespace ObsCamMove {
    CameraController::CameraController() : camera_moving_(false) {
    }

    obs_sceneitem_t* CameraController::find_active_camera_item() const {
        if (camera_names_.empty()) {
            log(LogLevel::DEBUG, "Unable to find any camera items");
            return nullptr;
        }

        const auto scene_source = obs_frontend_get_current_scene();
        if (scene_source == nullptr) {
            log(LogLevel::DEBUG, "Unable to find scene source for the camera");
            return nullptr;
        }

        const auto scene = obs_scene_from_source(scene_source);
        obs_source_release(scene_source); // Important: Releasing the source to prevent memory leaks!
        if (scene == nullptr) {
            log(LogLevel::DEBUG, "Unable to get scene for scene source");
            return nullptr;
        }

        for (const auto& sourceName : camera_names_) {
            if (obs_sceneitem_t* item = obs_scene_find_source(scene, sourceName.c_str())) {
                return item;
            }
        }

        return nullptr;
    }

    void CameraController::set_camera_names(std::vector<String> names) {
        camera_names_.clear();
        camera_names_.insert_range(names);

        const auto new_camera_names= join_strings(names, [](String s) {
            return std::format("\"{}\"", s);
        });
        log(LogLevel::INFO, "Setting camera names: " + new_camera_names);
    }

    String CameraController::get_camera_name() const {
        if (camera_names_.empty()) {
            return "ERROR: Camera names not set";
        }

        const auto camera_item = find_active_camera_item();
        if (camera_item == nullptr) {
            return "ERROR: Camera item not found";
        }

        const auto source = obs_sceneitem_get_source(camera_item);
        if (source == nullptr) {
            return "ERROR: Source item not found";
        }

        const auto name = obs_source_get_name(source);
        if (name == nullptr) {
            return "ERROR: Name for source item not found";
        }

        return String(name);
    }

    void CameraController::move_to(int x, int y, int duration) {
        auto cameraItem = find_active_camera_item();
        if (cameraItem == nullptr) {
            log(LogLevel::WARN, "Can't find active camera; moving is not possible!");
            return;
        }

        if (camera_moving_.load()) {
            log(LogLevel::WARN, "Camera moving is already active");
            return;
        }

        std::thread([=, this]() {
            camera_moving_.store(true);

            // Get current camera position
            vec2 start_pos;
            obs_sceneitem_get_pos(cameraItem, &start_pos);

            int start_x = static_cast<int>(start_pos.x);
            int start_y = static_cast<int>(start_pos.y);

            // Number of steps based on the duration and resolution
            const int fps = 60;
            const int steps = duration / (1000 / fps);
            const float step_duration = duration / static_cast<float>(steps);

            for (int i = 0; i < steps; i++) {
                // Lerp (linear interpolation)
                float t = i / static_cast<float>(steps);
                float current_x = static_cast<int>(start_x + t * (x - start_x));
                float current_y = static_cast<int>(start_y + t * (y - start_y));

                vec2 new_pos = { static_cast<float>(current_x), static_cast<float>(current_y) };
                obs_sceneitem_set_pos(cameraItem, &new_pos);

                // Wait before the next step is executed
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(step_duration)));
            }

            camera_moving_.store(false);
        }).detach();
    }

    void CameraController::move_by(int dx, int dy, int duration) {
        auto cameraItem = find_active_camera_item();
        if (cameraItem == nullptr) {
            log(LogLevel::WARN, "Can't find active camera; moving is not possible!");
            return;
        }

        if (camera_moving_.load()) {
            log(LogLevel::WARN, "Camera moving is already active");
            return;
        }

        vec2 start_pos;
        obs_sceneitem_get_pos(cameraItem, &start_pos);

        float start_x = start_pos.x;
        float start_y = start_pos.y;
        float target_x = start_x + dx;
        float target_y = start_y + dy;

        move_to(target_x, target_y, duration);
    }
}
