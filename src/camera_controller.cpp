#include "camera_controller.h"
#include "logger.h"
#include <obs.h>

namespace ObsCamMove {
    CameraController::CameraController() : camera_moving_(false) {
    }

    obs_scene_t* CameraController::find_active_scene() const {
        obs_source_t* source = nullptr;
        obs_scene_t* activeScene = nullptr;

        obs_enum_sources([](void* param, obs_source_t* source) -> bool {
            if (obs_source_get_type(source) == OBS_SOURCE_TYPE_SCENE) {
                if (obs_source_active(source)) {
                    *(obs_source_t**)param = obs_source_get_ref(source);
                    return false; // Stop enumeration
                }
            }
            return true;
        }, &source);

        if (source) {
            activeScene = obs_scene_from_source(source);
            obs_source_release(source);
        }

        return activeScene;
    }

    obs_sceneitem_t* CameraController::find_active_camera_item() const {
        if (camera_names_.empty()) {
            return nullptr;
        }

        auto scene = find_active_scene();
        if (scene == nullptr) {
            return nullptr;
        }

        for (const auto& sourceName : camera_names_) {
            obs_sceneitem_t* item = obs_scene_find_source(scene, sourceName.c_str());
            if (item) {
                return item;
            }
        }

        return nullptr;
    }

    void CameraController::set_camera_names(std::vector<String> names) {
        camera_names_.clear();
        camera_names_.insert_range(names);
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
