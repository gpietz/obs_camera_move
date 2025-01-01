#pragma once

#include "prerequisites.h"
#include <unordered_set>
#include <string>
#include <tuple>
#include <atomic>
#include <obs-module.h>

namespace ObsCamMove {
    enum class CameraEasingType : uint8_t {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
    };

    class CameraController {
    public:
        static CameraController& getInstance() {
            static CameraController instance;
            return instance;
        }

        void set_camera_names(std::vector<String> names);

        //! Moves the webcam to the specified position (x, y) over the specified duration in milliseconds.
        void move_to(int x, int y, int duration);
        //! Moves the webcam relative to the current position by (dx, dy) over the specified duration.
        void move_by(int dx, int dy, int duration);

        /**
        void follow(std::string objectId, int duration, bool reset);
        void stop_movement();
        bool is_moving() const;

        void scale_to(int width, int heigth, int duration);
        void zoom(int factor, int duration);

        void fade_in(int duration);
        void fade_out(int duration);
        void show();
        void hide();

        std::tuple<int, int> get_position() const;
        std::tuple<int, int> get_scale() const;
        bool get_visibiliy() const;

        void set_bounds(int min_x, int min_y, int max_x, int max_y);
        void set_speed(int speed);
        void set_easing(CameraEasingType easing);
        void set_position(int x, int y);

        void rotate(int angle, int duration, bool reset);
        void reset();
        void lock_position(bool lock);
        **/

    private:
        std::unordered_set<std::string> camera_names_;
        std::atomic<bool> camera_moving_;

        CameraController();

        obs_scene_t* find_active_scene() const;
        obs_sceneitem_t* find_active_camera_item() const;
    };
}
