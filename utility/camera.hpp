#ifndef UTILITY_CAMERA_HPP
#define UTILITY_CAMERA_HPP

#include <functional>
#include <iostream>
#include <type_traits>

// For python style string formatting
#include "fmt/format.h"

// For matrix and vector arithmetic
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// clang-format off
// Must include glad first
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

namespace utility {
namespace camera {

    class Camera {
    public:
        Camera(const int& width, const int& height, const float& near_plane, const float& far_plane) {
            this->width      = width;
            this->height     = height;
            this->near_plane = near_plane;
            this->far_plane  = far_plane;

            fov          = 45.0f;
            aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
            forward      = glm::vec3(0.0f, 0.0f, -1.0f);
            up           = glm::vec3(0.0f, 1.0f, 0.0f);
            position     = glm::vec3(0.0f, 0.0f, 3.0f);
            right        = glm::normalize(glm::cross(forward, up));

            first_mouse          = true;
            last_mouse_pos       = glm::vec2(static_cast<float>(width) * 0.5f, static_cast<float>(height) * 0.5f);
            rotation_sensitivity = 0.005f;
            movement_sensitivity = 0.005f;
        }

        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
            if (fov >= 1.0f && fov <= 45.0f) {
                fov -= static_cast<float>(yoffset);
            }
            fov = std::min(std::max(1.0f, fov), 45.0f);
        }
        void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
            // prevent erratic movements when the mouse first enters the screen
            if (first_mouse) {
                last_mouse_pos = glm::vec2(static_cast<float>(xpos), static_cast<float>(ypos));
                first_mouse    = false;
            }

            glm::vec2 current_mouse_pos(static_cast<float>(xpos), static_cast<float>(ypos));

            // invert y-ccordinates since they range from top to bottom
            glm::vec2 offset(current_mouse_pos.x - last_mouse_pos.x, last_mouse_pos.y - current_mouse_pos.y);
            offset *= rotation_sensitivity;

            last_mouse_pos = current_mouse_pos;

            // update camera rotation
            orientation += offset;

            // clamp pitch to [-89, 89] degrees
            // weird things happen when pitch is at +/- 90
            orientation.y = std::min(std::max(-89.0f, orientation.y), 89.0f);

            const float cos_pitch = std::cos(orientation.y);
            const float sin_pitch = std::sin(orientation.y);
            const float cos_yaw   = std::cos(orientation.x);
            const float sin_yaw   = std::sin(orientation.x);
            forward               = glm::normalize(glm::vec3(cos_pitch * cos_yaw, sin_pitch, cos_pitch * sin_yaw));
            right                 = glm::normalize(glm::cross(forward, up));
        }

        // glfw: whenever the window size changed (by OS or user resize) this callback function executes
        // ---------------------------------------------------------------------------------------------
        void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
            // make sure the viewport matches the new window dimensions; note that width and
            // height will be significantly larger than specified on retina displays.
            glViewport(0, 0, width, height);
            this->width  = width;
            this->height = height;
            aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
        }

        glm::mat4 get_view_transform() {
            return glm::lookAt(position, position + forward, up);
        }
        glm::mat4 get_clip_transform() {
            return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
        }

        void set_movement_sensitivity(const float& sensitivity) {
            movement_sensitivity = sensitivity;
        }
        void set_rotation_sensitivity(const float& sensitivity) {
            rotation_sensitivity = sensitivity;
        }
        void move_left() {
            position -= right * movement_sensitivity;
        }
        void move_right() {
            position += right * movement_sensitivity;
        }
        void move_up() {
            position += forward * movement_sensitivity;
        }
        void move_down() {
            position -= forward * movement_sensitivity;
        }

    private:
        int width;
        int height;
        float near_plane;
        float far_plane;

        float fov;
        float aspect_ratio;
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 position;
        glm::vec3 right;
        glm::vec2 orientation;

        bool first_mouse;
        glm::vec2 last_mouse_pos;
        float rotation_sensitivity;
        float movement_sensitivity;
    };

    // Template hackiness to allow us to pass a member function to the GLFW API as a C-style function pointer
    // https://stackoverflow.com/a/39524069
    template <typename T>
    struct ActualType {
        typedef T type;
    };
    template <typename T>
    struct ActualType<T*> {
        typedef typename ActualType<T>::type type;
    };

    template <typename T, unsigned int n, typename CallerType>
    struct Callback;

    template <typename Ret, typename... Params, unsigned int n, typename CallerType>
    struct Callback<Ret(Params...), n, CallerType> {
        typedef Ret (*ret_cb)(Params...);
        template <typename... Args>
        static Ret callback(Args... args) {
            func(args...);
        }

        static ret_cb getCallback(std::function<Ret(Params...)> fn) {
            func = fn;
            return static_cast<ret_cb>(Callback<Ret(Params...), n, CallerType>::callback);
        }

        static std::function<Ret(Params...)> func;
    };

    template <typename Ret, typename... Params, unsigned int n, typename CallerType>
    std::function<Ret(Params...)> Callback<Ret(Params...), n, CallerType>::func;

// Call this as (example for the GLFW scroll callback)
// CCallbackWrapper(GLFWscrollfun, Camera)(std::bind(&Camera::scroll_back, &camera, _1, _2, _2))
#define CCallbackWrapper(ptrtype, callertype)                                                                          \
    utility::camera::Callback<utility::camera::ActualType<ptrtype>::type, __COUNTER__, callertype>::getCallback
}  // namespace camera
}  // namespace utility


#endif  // UTILITY_CAMERA_HPP
