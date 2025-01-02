#include "camera_controller.h"
#include "camera_easing.h"
#include "logger.h"
#include "string_utils.h"
#include <obs.h>
#include <obs-frontend-api.h>

namespace ObsCamMove {
    CameraController::CameraController() : camera_moving_(false) {
    }

    String CameraController::log_error(const String& error_message) {
        log(LogLevel::ERROR, error_message);
        return "ERROR: " + error_message;
    }

    String CameraController::get_camera_value(const GetCameraValueCallback &get_value_function) const {
        if (camera_names_.empty()) {
            return log_error("Unable to find any camera items!");
        }

        const auto scene_source = obs_frontend_get_current_scene();
        if (!scene_source) {
            return log_error("No current scene available!");
        }

        const auto scene = obs_scene_from_source(scene_source);
        obs_source_release(scene_source); // Important: Releasing the source to prevent memory leaks!
        if (!scene) {
            return log_error("Current source is not a scene!");
        }

        obs_sceneitem_t* camera = nullptr;
        for (const auto& source_name : camera_names_) {
            if (obs_sceneitem_t* item = obs_scene_find_source(scene, source_name.c_str())) {
                camera = item;
                break;
            }
        }

        if (!camera) {
            return log_error("No camera in current scene found!");
        }

        const auto camera_source = obs_sceneitem_get_source(camera);
        if (camera_source == nullptr) {
            return log_error("ERROR: Source item for camera not found");
        }

        return get_value_function(scene, camera, camera_source);
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
        return get_camera_value([](obs_scene_t*, obs_sceneitem_t*, const obs_source_t* camera_source) {
            return obs_source_get_name(camera_source);
        });
    }

    void CameraController::move_to(const int x, const int y, const int duration, const u8 easing) {
        const auto cameraItem = find_active_camera_item();
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

            float start_x = start_pos.x;
            float start_y = start_pos.y;

            log(LogLevel::DEBUG, std::format("Starting pos: {}, {}", start_x, start_y));

            // Calculate distance
            const float distance = std::sqrt(std::pow(x - start_x, 2) + std::pow(y - start_y, 2));

            // Retrieve FPS from OBS
            double fps = obs_get_active_fps();
            if (fps <= 0.0) {
                log(LogLevel::ERROR, "Failed to retrieve FPS value from OBS. Defaulting to 60.");
                fps = 60.0;
            }

            // Speed and FPS settings
            constexpr float speed_per_second = 300.0f; // Pixels per Second
            const float speed_per_frame = speed_per_second / static_cast<float>(fps);

            // Calculate number of steps and duration per step
            const int steps = std::max(1, static_cast<int>(distance / speed_per_frame));
            const float step_duration = duration / static_cast<float>(steps);

            log(LogLevel::DEBUG, std::format("Number of steps: {}", steps));
            log(LogLevel::DEBUG, std::format("FPS: {}", fps));
            log(LogLevel::DEBUG, std::format("Step duration: {}", step_duration));

            // Convert value to easing type
            const auto easing_type = CameraEasing::to_camera_easing_type(easing);

            for (int i = 0; i < steps; i++) {
                float t = i / static_cast<float>(steps);

                // Easing
                t = CameraEasing::calculate(easing_type, t);

                // Calculate new camera position per step
                vec2 new_pos = {
                    start_x + t * (x - start_x),
                    start_y + t * (y - start_y)
                };

                // Update camera position
                obs_sceneitem_set_pos(cameraItem, &new_pos);

                // Wait before the next step is executed
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(step_duration)));
            }

            camera_moving_.store(false);
        }).detach();
    }

    void CameraController::move_by(const int dx, const int dy, const int duration, const u8 easing) {
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

        const float start_x = start_pos.x;
        const float start_y = start_pos.y;
        const float target_x = start_x + dx;
        const float target_y = start_y + dy;

        move_to(target_x, target_y, duration, easing);
    }

    String CameraController::get_position() const {
        return get_camera_value([](obs_scene*, const obs_sceneitem_t* camera, const obs_source_t*) {
            obs_transform_info transform;
            obs_sceneitem_get_info2(camera, &transform);
            const auto x = transform.pos.x;
            const auto y = transform.pos.y;
            return std::format("camera-position: x={}, y={}", x, y);
        });
    }
}
