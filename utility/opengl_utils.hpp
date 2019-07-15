#ifndef UTILITY_OPENGL_UTILS_HPP
#define UTILITY_OPENGL_UTILS_HPP

#include <array>

// clang-format off
// Must include glad first
#include "glad/glad.h"
// clang-format on

namespace utility {

namespace gl {
    struct vertex_array {
        vertex_array() {
            glGenVertexArrays(1, &VAO);
            bound = false;
        }
        ~vertex_array() {
            glDeleteVertexArrays(1, &VAO);
        }

        void bind() {
            if (!bound) {
                glBindVertexArray(VAO);
                bound = true;
            }
        }
        void unbind() {
            if (bound) {
                glBindVertexArray(0);
                bound = false;
            }
        }
        operator unsigned int() const {
            return VAO;
        }

    private:
        unsigned int VAO;
        bool bound;
    };
    struct vertex_buffer {
        vertex_buffer() {
            glGenBuffers(1, &VBO);
            bound = false;
        }
        ~vertex_buffer() {
            glDeleteBuffers(1, &VBO);
        }

        void bind() {
            if (!bound) {
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                bound = true;
            }
        }
        void unbind() {
            if (bound) {
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                bound = false;
            }
        }

        template <int N>
        void copy_data(const std::array<float, N>& vertices, const unsigned int& draw_method) {
            if (!bound) {
                bind();
            }
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], draw_method);
        }

        operator unsigned int() const {
            return VBO;
        }

    private:
        unsigned int VBO;
        bool bound;
    };
}  // namespace gl
}  // namespace utility


#endif  // UTILITY_OPENGL_UTILS_HPP
