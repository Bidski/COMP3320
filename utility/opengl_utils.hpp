#ifndef UTILITY_OPENGL_UTILS_HPP
#define UTILITY_OPENGL_UTILS_HPP

#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// For python style string formatting
#include "fmt/format.h"

// clang-format off
#include "glad/glad.h"
// clang-format on

#include "opengl_error_category.hpp"

namespace utility {
namespace gl {
    inline void throw_gl_error(const int& code, const std::string& msg) {
        if (code != GL_NO_ERROR) {
            throw std::system_error(code, opengl_error_category(), msg);
        }
    }

    inline std::string get_shader_string(const unsigned int& shader_type) {
        switch (shader_type) {
            case GL_VERTEX_SHADER: return "GL_VERTEX_SHADER"; break;
            case GL_FRAGMENT_SHADER: return "GL_FRAGMENT_SHADER"; break;
            default: return "UNKNOWN_SHADER"; break;
        }
    }

    struct shader_program {
        shader_program() {
            // Create a shader program
            program = glCreateProgram();
            throw_gl_error(glGetError(), fmt::format("Failed to create shader program"));
        }
        ~shader_program() {
            shaders.clear();
            glDeleteProgram(program);
        }

        void add_shader(const std::string& shader_source, const unsigned int& shader_type) {
            // Load shader source code from the specified file
            // See: http://stackoverflow.com/a/116228
            std::ifstream data(shader_source, std::ios::in);
            if (!data.good()) {
                throw std::system_error(std::error_code(ENOENT, std::system_category()),
                                        fmt::format("Failed to open shader file '{}'", shader_source));
            }
            std::stringstream stream;
            stream << data.rdbuf();
            std::string code(stream.str());

            unsigned int shader_id = glCreateShader(shader_type);
            throw_gl_error(glGetError(), fmt::format("Failed to create shader for {}", shader_source));

            // glShaderSource expects an array of strings
            const char* shader_src = code.c_str();
            glShaderSource(shader_id, 1, &shader_src, nullptr);
            throw_gl_error(glGetError(), fmt::format("Failed to load shader source for {}", shader_source));

            glCompileShader(shader_id);
            throw_gl_error(glGetError(),
                           fmt::format("Failed to compile shader with type {}", get_shader_string(shader_type)));

            // Check for compile errors
            int success;
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
            throw_gl_error(glGetError(),
                           fmt::format("Failed to get shader compile status for shader with type {}",
                                       get_shader_string(shader_type)));

            if (success == GL_FALSE) {
                std::string shader_type_str(get_shader_string(shader_type));
                int length = 0;
                glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
                std::string info_log(length, 0);
                glGetShaderInfoLog(shader_id, length, nullptr, &info_log[0]);
                throw_gl_error(glGetError(),
                               fmt::format("Failed to get shader compile log for shader with type {}",
                                           get_shader_string(shader_type)));
                throw std::system_error(
                    std::error_code(EINVAL, std::system_category()),
                    fmt::format("Shader Compilation Failed:\nShader Type: {}\nLog: {}", shader_type_str, info_log));
            }

            shaders.push_back(shader_id);
        }

        void link() {
            if (shaders.empty()) {
                throw std::system_error(std::error_code(EINVAL, std::system_category()),
                                        "Can't link a program with no shaders.");
            }
            if (glIsProgram(program) == GL_TRUE) {
                // Attach each of the provided shaders to the progrm
                for (auto shader : shaders) {
                    if (glIsShader(shader) == GL_TRUE) {
                        glAttachShader(program, shader);
                        throw_gl_error(glGetError(), fmt::format("Failed to attach shader to program"));
                    }
                    else {
                        std::cout << fmt::format("Shader {} is invalid", static_cast<unsigned int>(shader))
                                  << std::endl;
                    }
                }

                // Link all of the shaders together
                glLinkProgram(program);
                throw_gl_error(glGetError(), fmt::format("Failed to link shader program"));

                // Check for linking errors
                int success;
                glGetProgramiv(program, GL_LINK_STATUS, &success);
                throw_gl_error(glGetError(), fmt::format("Failed to get shader program link status"));

                if (success == GL_FALSE) {
                    int length = 0;
                    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
                    std::string info_log(length, 0);
                    glGetProgramInfoLog(program, length, nullptr, &info_log[0]);
                    throw_gl_error(glGetError(), fmt::format("Failed to get shader program link log"));
                    throw std::system_error(std::error_code(EINVAL, std::system_category()),
                                            fmt::format("Program Linking Failed:\nLog: {}", info_log));
                }

                // Program is linked, we can discard the shaders now
                for (auto& shader : shaders) {
                    glDeleteShader(shader);
                }
                shaders.clear();
            }
            else {
                throw std::system_error(std::error_code(EINVAL, std::system_category()), "Invalid Program!");
            }
        }

        void use() {
            glUseProgram(program);
        }
        void release() {
            glUseProgram(0);
        }

        void set_uniform(const std::string& uniform, const std::array<float, 4>& value) {
            glUseProgram(program);
            if (uniforms.find(uniform) == uniforms.end()) {
                uniforms[uniform] = glGetUniformLocation(program, uniform.c_str());
                throw_gl_error(glGetError(), fmt::format("Failed to find uniform '{}'", uniform));
            }
            glUniform4f(uniforms[uniform], value[0], value[1], value[2], value[3]);
            throw_gl_error(glGetError(),
                           fmt::format("Failed to set uniform '{}' at location {}", uniform, uniforms[uniform]));
        }

        operator unsigned int() {
            return program;
        }

    private:
        std::vector<unsigned int> shaders;
        unsigned int program;
        std::map<std::string, int> uniforms;
    };  // namespace gl

    struct vertex_array {
        vertex_array() {
            glGenVertexArrays(1, &VAO);
        }
        ~vertex_array() {
            glDeleteVertexArrays(1, &VAO);
        }

        void bind() {
            glBindVertexArray(VAO);
        }
        void unbind() {
            glBindVertexArray(0);
        }

        template <typename Scalar>
        void add_vertex_attrib(const unsigned int& location,
                               const int& size,
                               const int& width,
                               const unsigned int& type,
                               const bool& normalised,
                               const unsigned int& offset) {
            bind();
            glVertexAttribPointer(location,
                                  size,
                                  type,
                                  normalised ? GL_TRUE : GL_FALSE,
                                  width * sizeof(Scalar),
                                  (void*) (offset * sizeof(Scalar)));
            glEnableVertexAttribArray(location);
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

        void bind() {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
        }
        void unbind() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        template <int N>
        void copy_data(const std::array<float, N>& vertices, const unsigned int& draw_method) {
            bind();
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], draw_method);
        }

        operator unsigned int() const {
            return VBO;
        }

    private:
        unsigned int VBO;
    };
    struct element_buffer {
        element_buffer() {
            glGenBuffers(1, &EBO);
        }
        ~element_buffer() {
            glDeleteBuffers(1, &EBO);
        }

        void bind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        }
        void unbind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        template <int N>
        void copy_data(const std::array<unsigned int, N>& indices, const unsigned int& draw_method) {
            bind();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], draw_method);
        }

        operator unsigned int() const {
            return EBO;
        }

    private:
        unsigned int EBO;
    };
}  // namespace gl
}  // namespace utility


#endif  // UTILITY_OPENGL_UTILS_HPP
