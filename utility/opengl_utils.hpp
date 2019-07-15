#ifndef UTILITY_OPENGL_UTILS_HPP
#define UTILITY_OPENGL_UTILS_HPP

// clang-format off
// Must include glad first
#include "glad/glad.h"
// clang-format on

namespace utility {

namespace gl {
    struct vertex_array {
        vertex_array() {
            glGenVertexArrays(1, &VAO);
        }
        ~vertex_array() {
            glDeleteVertexArrays(1, &VAO);
        }

        operator unsigned int() const {
            return VAO;
        }

    private:
        unsigned int VAO;
    };
    struct vertex_buffer {
        vertex_buffer() {
            glGenBuffers(1, &VBO);
        }
        ~vertex_buffer() {
            glDeleteBuffers(1, &VBO);
        }
        void unbind() {
            glDeleteBuffers(1, &VBO);
        }

        operator unsigned int() const {
            return VBO;
        }

    private:
        unsigned int VBO;
    };
}  // namespace gl
}  // namespace utility


#endif  // UTILITY_OPENGL_UTILS_HPP
