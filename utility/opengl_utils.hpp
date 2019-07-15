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

        template <typename Scalar>
        void add_vertex_attrib(const unsigned int& location,
                               const int& size,
                               const unsigned int& type,
                               const bool& normalised,
                               Scalar* offset) {
            if (!bound) {
                bind();
            }
            glVertexAttribPointer(location,
                                  size,
                                  type,
                                  normalised ? GL_TRUE : GL_FALSE,
                                  size * sizeof(Scalar),
                                  static_cast<void*>(offset));
            glEnableVertexAttribArray(location);
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
    struct element_buffer {
        element_buffer() {
            glGenBuffers(1, &EBO);
            bound = false;
        }
        ~element_buffer() {
            glDeleteBuffers(1, &EBO);
        }

        void bind() {
            if (!bound) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                bound = true;
            }
        }
        void unbind() {
            if (bound) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                bound = false;
            }
        }

        template <int N>
        void copy_data(const std::array<unsigned int, N>& indices, const unsigned int& draw_method) {
            if (!bound) {
                bind();
            }
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], draw_method);
        }

        operator unsigned int() const {
            return EBO;
        }

    private:
        unsigned int EBO;
        bool bound;
    };
}  // namespace gl
}  // namespace utility


#endif  // UTILITY_OPENGL_UTILS_HPP
