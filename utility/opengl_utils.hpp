#ifndef UTILITY_OPENGL_UTILS_HPP
#define UTILITY_OPENGL_UTILS_HPP

#include <array>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// For python style string formatting
#include "fmt/format.h"

// For image/texture loading
#include "SOIL/SOIL.h"

// For matrix and vector arithmetic
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// clang-format off
#include "glad/glad.h"
// clang-format on

#include "opengl_error_category.hpp"

namespace utility {
namespace gl {
    // A wrapper for throwing exceptions based on OpenGL error codes
    // If no error is detector (code == GL_NO_ERROR) then nothing is thrown
    // Example: throw_gl_error(glGetError(), "Error doing something important");
    // -------------------------------------------------------------------------
    inline void throw_gl_error(const int& code, const std::string& msg) {
        if (code != GL_NO_ERROR) {
            throw std::system_error(code, opengl_error_category(), msg);
        }
    }

    // Create a smart enum to wrap shader enum types
    // ---------------------------------------------
    struct ShaderType {
        enum Value {
            VERTEX_SHADER   = GL_VERTEX_SHADER,
            FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
            GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
            UNKNOWN
        };
        Value value;
        ShaderType() {
            value = Value::UNKNOWN;
        }
        ShaderType(const unsigned int& shader_type) {
            switch (shader_type) {
                case GL_VERTEX_SHADER: value = Value::VERTEX_SHADER; break;
                case GL_FRAGMENT_SHADER: value = Value::FRAGMENT_SHADER; break;
                case GL_GEOMETRY_SHADER: value = Value::GEOMETRY_SHADER; break;
                default: throw_gl_error(GL_INVALID_ENUM, fmt::format("Invalid shader type '{}'", shader_type));
            }
        }
        ShaderType(const std::string& shader_type) {
            if (shader_type == "GL_VERTEX_SHADER" || shader_type == "VERTEX_SHADER") {
                value = Value::VERTEX_SHADER;
            }
            else if (shader_type == "GL_FRAGMENT_SHADER" || shader_type == "FRAGMENT_SHADER") {
                value = Value::FRAGMENT_SHADER;
            }
            else if (shader_type == "GL_GEOMETRY_SHADER" || shader_type == "GEOMETRY_SHADER") {
                value = Value::GEOMETRY_SHADER;
            }
            else {
                throw_gl_error(GL_INVALID_ENUM, fmt::format("Invalid shader type '{}'", shader_type));
            }
        }

        operator unsigned int() const {
            return value;
        }
        operator std::string() const {
            switch (value) {
                case Value::VERTEX_SHADER: return "VERTEX_SHADER";
                case Value::FRAGMENT_SHADER: return "FRAGMENT_SHADER";
                case Value::GEOMETRY_SHADER: return "GEOMETRY_SHADER";
                default: throw_gl_error(GL_INVALID_ENUM, fmt::format("Invalid shader type '{}'", value));
            }
        }
    };

    // Create a smart enum to wrap texture enum types
    // ----------------------------------------------
    struct TextureType {
        enum Value {
            TEXTURE_1D                   = GL_TEXTURE_1D,
            TEXTURE_1D_ARRAY             = GL_TEXTURE_1D_ARRAY,
            TEXTURE_2D                   = GL_TEXTURE_2D,
            TEXTURE_2D_ARRAY             = GL_TEXTURE_2D_ARRAY,
            TEXTURE_2D_MULTISAMPLE       = GL_TEXTURE_2D_MULTISAMPLE,
            TEXTURE_2D_MULTISAMPLE_ARRAY = GL_TEXTURE_2D_MULTISAMPLE_ARRAY,
            TEXTURE_3D                   = GL_TEXTURE_3D,
            UNKNOWN
        };
        Value value;
        TextureType() {
            value = Value::UNKNOWN;
        }
        TextureType(const unsigned int& texture_type) {
            switch (texture_type) {
                case GL_TEXTURE_1D: value = Value::TEXTURE_1D; break;
                case GL_TEXTURE_1D_ARRAY: value = Value::TEXTURE_1D_ARRAY; break;
                case GL_TEXTURE_2D: value = Value::TEXTURE_2D; break;
                case GL_TEXTURE_2D_ARRAY: value = Value::TEXTURE_2D_ARRAY; break;
                case GL_TEXTURE_2D_MULTISAMPLE: value = Value::TEXTURE_2D_MULTISAMPLE; break;
                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: value = Value::TEXTURE_2D_MULTISAMPLE_ARRAY; break;
                case GL_TEXTURE_3D: value = Value::TEXTURE_3D; break;
                default: throw_gl_error(GL_INVALID_ENUM, fmt::format("Invalid texture type '{}'", texture_type));
            }
        }
        TextureType(const std::string& texture_type) {
            if (texture_type == "TEXTURE_1D" || texture_type == "GL_TEXTURE_1D") {
                value = Value::TEXTURE_1D;
            }
            else if (texture_type == "TEXTURE_1D_ARRAY" || texture_type == "GL_TEXTURE_1D_ARRAY") {
                value = Value::TEXTURE_1D_ARRAY;
            }
            else if (texture_type == "TEXTURE_2D" || texture_type == "GL_TEXTURE_2D") {
                value = Value::TEXTURE_2D;
            }
            else if (texture_type == "TEXTURE_2D_ARRAY" || texture_type == "GL_TEXTURE_2D_ARRAY") {
                value = Value::TEXTURE_2D_ARRAY;
            }
            else if (texture_type == "TEXTURE_2D_MULTISAMPLE" || texture_type == "GL_TEXTURE_2D_MULTISAMPLE") {
                value = Value::TEXTURE_2D_MULTISAMPLE;
            }
            else if (texture_type == "TEXTURE_2D_MULTISAMPLE_ARRAY"
                     || texture_type == "GL_TEXTURE_2D_MULTISAMPLE_ARRAY") {
                value = Value::TEXTURE_2D_MULTISAMPLE_ARRAY;
            }
            else if (texture_type == "TEXTURE_3D" || texture_type == "GL_TEXTURE_3D") {
                value = Value::TEXTURE_3D;
            }
            else {
                throw_gl_error(GL_INVALID_ENUM, fmt::format("Invalid texture type '{}'", texture_type));
            }
        }

        operator unsigned int() const {
            return value;
        }
        operator std::string() const {
            switch (value) {
                case GL_TEXTURE_1D: return "TEXTURE_1D";
                case GL_TEXTURE_1D_ARRAY: return "TEXTURE_1D_ARRAY";
                case GL_TEXTURE_2D: return "TEXTURE_2D";
                case GL_TEXTURE_2D_ARRAY: return "TEXTURE_2D_ARRAY";
                case GL_TEXTURE_2D_MULTISAMPLE: return "TEXTURE_2D_MULTISAMPLE";
                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "TEXTURE_2D_MULTISAMPLE_ARRAY";
                case GL_TEXTURE_3D: return "TEXTURE_3D";
                default: throw_gl_error(GL_INVALID_ENUM, fmt::format("Invalid texture type '{}'", value));
            }
        }
    };

    // Create a wrapper for OpenGL shader programs
    // -------------------------------------------
    struct shader_program {
        // Create an empty program
        shader_program() {
            // Create a shader program
            program = glCreateProgram();
            throw_gl_error(glGetError(), fmt::format("Failed to create shader program"));
        }
        // Clean up all references
        ~shader_program() {
            for (auto& shader : shaders) {
                glDeleteShader(shader);
            }
            shaders.clear();
            glDeleteProgram(program);
        }

        // Add shader source code from a file
        // ------------------------------------------------------------------------------------
        // shader_source: Path to file that contains the shader source code
        // shader_type: The type of the shader that is being added (vertex, fragment, geometry)
        // ------------------------------------------------------------------------------------
        void add_shader(const std::string& shader_source, const ShaderType& shader_type) {
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

            // Create the shader
            unsigned int shader_id = glCreateShader(shader_type);
            throw_gl_error(glGetError(), fmt::format("Failed to create shader for {}", shader_source));

            // glShaderSource expects an array of strings
            const char* shader_src = code.c_str();
            glShaderSource(shader_id, 1, &shader_src, nullptr);
            throw_gl_error(glGetError(), fmt::format("Failed to load shader source for {}", shader_source));

            // Compile the shader
            glCompileShader(shader_id);
            throw_gl_error(glGetError(), fmt::format("Failed to compile shader with type {}", shader_type));

            // Check for compile errors
            int success;
            glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
            throw_gl_error(glGetError(),
                           fmt::format("Failed to get shader compile status for shader with type {}", shader_type));

            if (success == GL_FALSE) {
                std::string shader_type_str(shader_type);
                int length = 0;
                glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &length);
                std::string info_log(length, 0);
                glGetShaderInfoLog(shader_id, length, nullptr, &info_log[0]);
                throw_gl_error(glGetError(),
                               fmt::format("Failed to get shader compile log for shader with type {}", shader_type));
                throw std::system_error(
                    std::error_code(EINVAL, std::system_category()),
                    fmt::format("Shader Compilation Failed:\nShader Type: {}\nLog: {}", shader_type_str, info_log));
            }

            // No errors, add shader to list of all shaders
            shaders.push_back(shader_id);
        }

        // Link all the shaders into a shader program
        // ------------------------------------------
        void link() {
            // Make sure we actually have some shaders to link together
            if (shaders.empty()) {
                throw std::system_error(std::error_code(EINVAL, std::system_category()),
                                        "Can't link a program with no shaders.");
            }

            // Make sure the program is valid
            if (glIsProgram(program) == GL_TRUE) {
                // Attach each of the provided shaders to the progrm
                for (auto shader : shaders) {
                    if (glIsShader(shader) == GL_TRUE) {
                        glAttachShader(program, shader);
                        throw_gl_error(glGetError(), fmt::format("Failed to attach shader to program"));
                    }
                    else {
                        throw_gl_error(GL_INVALID_VALUE,
                                       fmt::format("Shader {} is invalid", static_cast<unsigned int>(shader)));
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

        // Make this program the currently active program
        // ----------------------------------------------
        void use() {
            glUseProgram(program);
        }
        // Deactive this program
        // ---------------------
        void release() {
            glUseProgram(0);
        }

        // Get the location of a named uniform in the program
        // --------------------------------------------------
        // uniform: The name of the uniform to find
        // --------------------------------------------------
        int get_uniform_location(const std::string& uniform) {
            glUseProgram(program);
            if (uniforms.find(uniform) == uniforms.end()) {
                uniforms[uniform] = glGetUniformLocation(program, uniform.c_str());
                throw_gl_error(glGetError(), fmt::format("Failed to find uniform '{}'", uniform));
            }
            return uniforms[uniform];
        }
        // Different overloads for setting a named uniform to the provided value
        // ---------------------------------------------------------------------
        // uniform: The name of the uniform to set
        // value: The value to set the uniform to
        // ---------------------------------------------------------------------
        void set_uniform(const std::string& uniform, const glm::mat4& value) {
            glUniformMatrix4fv(get_uniform_location(uniform), 1, GL_FALSE, glm::value_ptr(value));
            throw_gl_error(glGetError(),
                           fmt::format("Failed to set uniform '{}' at location {}", uniform, uniforms[uniform]));
        }
        void set_uniform(const std::string& uniform, const std::array<float, 4>& value) {
            glUniform4f(get_uniform_location(uniform), value[0], value[1], value[2], value[3]);
            throw_gl_error(glGetError(),
                           fmt::format("Failed to set uniform '{}' at location {}", uniform, uniforms[uniform]));
        }
        void set_uniform(const std::string& uniform, const int& value) {
            glUniform1i(get_uniform_location(uniform), value);
            throw_gl_error(glGetError(),
                           fmt::format("Failed to set uniform '{}' at location {}", uniform, uniforms[uniform]));
        }
        void set_uniform(const std::string& uniform, const float& value) {
            glUniform1f(get_uniform_location(uniform), value);
            throw_gl_error(glGetError(),
                           fmt::format("Failed to set uniform '{}' at location {}", uniform, uniforms[uniform]));
        }

        // Allow this program wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // --------------------------------------------------------
        operator unsigned int() {
            return program;
        }

    private:
        std::vector<unsigned int> shaders;
        unsigned int program;
        std::map<std::string, int> uniforms;
    };

    // Create a wrapper for OpenGL vertex arrays
    // -----------------------------------------
    struct vertex_array {
        // Create a single vertex array
        // ----------------------------
        vertex_array() {
            glGenVertexArrays(1, &VAO);
        }
        // Delete the vertex array
        // -----------------------
        ~vertex_array() {
            glDeleteVertexArrays(1, &VAO);
        }

        // Bind the vertex array and make it active
        // ----------------------------------------
        void bind() {
            glBindVertexArray(VAO);
        }
        // Deactivate the vertex array
        // ---------------------------
        void unbind() {
            glBindVertexArray(0);
        }

        // Add a vertex attribute to the vertex array
        // ------------------------------------------
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

        // Allow this vertex array wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // -------------------------------------------------------------
        operator unsigned int() const {
            return VAO;
        }

    private:
        unsigned int VAO;
    };

    // Create a wrapper for OpenGL vertex buffers
    // ------------------------------------------
    struct vertex_buffer {
        // Create a single vertex buffer
        // -----------------------------
        vertex_buffer() {
            glGenBuffers(1, &VBO);
        }
        // Delete the vertex buffer
        // ------------------------
        ~vertex_buffer() {
            glDeleteBuffers(1, &VBO);
        }

        // Bind the vertex buffer and make it active
        // -----------------------------------------
        void bind() {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
        }
        // Deactivate the vertex buffer
        // ----------------------------
        void unbind() {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        // Copy vertex buffer data to the GPU
        // ----------------------------------
        template <int N>
        void copy_data(const std::array<float, N>& vertices, const unsigned int& draw_method) {
            bind();
            glBufferData(GL_ARRAY_BUFFER, N * sizeof(float), &vertices[0], draw_method);
        }

        // Allow this vertex buffer wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // --------------------------------------------------------------
        operator unsigned int() const {
            return VBO;
        }

    private:
        unsigned int VBO;
    };

    // Create a wrapper for OpenGL element buffers
    // -------------------------------------------
    struct element_buffer {
        // Create a single element buffer
        // ------------------------------
        element_buffer() {
            glGenBuffers(1, &EBO);
        }
        // Delete the element buffer
        // -------------------------
        ~element_buffer() {
            glDeleteBuffers(1, &EBO);
        }

        // Bind the element buffer and make it active
        // ------------------------------------------
        void bind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        }
        // Deactivate the element buffer
        // -----------------------------
        void unbind() {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        // Copy the element buffer data to the GPU
        // ---------------------------------------
        template <int N>
        void copy_data(const std::array<unsigned int, N>& indices, const unsigned int& draw_method) {
            bind();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, N * sizeof(unsigned int), &indices[0], draw_method);
        }

        // Allow this element buffer wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // ---------------------------------------------------------------
        operator unsigned int() const {
            return EBO;
        }

    private:
        unsigned int EBO;
    };

    // Create a wrapper for OpenGL textures
    // ------------------------------------
    struct texture {
        // Create an uninitialsed texture
        // ------------------------------
        texture(const TextureType& texture_type) {
            glGenTextures(1, &tex);
            this->texture_type = texture_type;
            texture_data.clear();
        }
        // Create a texture and initialise it with the given image file
        // ------------------------------------------------------------
        // image: Path to the image file to load
        // texture_type: The type of the texture that we are loading
        // ------------------------------------------------------------
        texture(const std::string& image, const TextureType& texture_type) {
            glGenTextures(1, &tex);
            this->texture_type = texture_type;

            unsigned char* data = SOIL_load_image(image.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
            texture_data.clear();
            texture_data.assign(data, data + (width * height * channels));
            SOIL_free_image_data(data);
        }
        // Delete the texture
        // ------------------
        ~texture() {
            glDeleteTextures(1, &tex);
        }

        // Load texture data from the provided array
        // -------------------------------------------------
        // data: Array of bytes to load into the texture
        // width: Width of the texture data
        // height: Height of the texture data
        // channels: Number of channels in the texture data
        // -------------------------------------------------
        void load_data(const std::vector<unsigned char>& data,
                       const unsigned int& width,
                       const unsigned int& height,
                       const unsigned int& channels) {
            texture_data.clear();
            texture_data.assign(data.begin(), data.end());
            this->width    = width;
            this->height   = height;
            this->channels = channels;
        }
        // Load texture data from the provided array
        // -------------------------------------------------
        // data: Array of bytes to load into the texture
        // width: Width of the texture data
        // height: Height of the texture data
        // channels: Number of channels in the texture data
        // -------------------------------------------------
        void load_data(const unsigned char* data,
                       const unsigned int& width,
                       const unsigned int& height,
                       const unsigned int& channels) {
            texture_data.clear();
            texture_data.assign(data, data + (width * height * channels));
            this->width    = width;
            this->height   = height;
            this->channels = channels;
        }

        // Bind the texture and make it active
        // ---------------------------------------------
        // unit: The texture unit to bind the texture to
        // ---------------------------------------------
        void bind(const unsigned int& unit = GL_TEXTURE0) {
            glActiveTexture(unit);
            glBindTexture(texture_type, tex);
        }
        // Deactivate the texture
        // ----------------------
        void unbind() {
            glBindTexture(texture_type, 0);
        }

        // Load the texture data on to the GPU
        // -----------------------------------
        void generate(const unsigned int& mipmap_level, const unsigned int& pixel_type) {
            switch (texture_type) {
                case TextureType::TEXTURE_2D:
                    glTexImage2D(texture_type,
                                 mipmap_level,
                                 pixel_type,
                                 width,
                                 height,
                                 0,
                                 pixel_type,
                                 GL_UNSIGNED_BYTE,
                                 texture_data.data());
                    break;
                default:
                    throw_gl_error(GL_INVALID_OPERATION,
                                   fmt::format("Texture type '{}' currently not supported", texture_type));
            }
        }
        // Generate mipmapped textures
        // ---------------------------
        void generate_mipmap() {
            switch (texture_type) {
                case TextureType::TEXTURE_2D: glGenerateMipmap(GL_TEXTURE_2D); break;
                default:
                    throw_gl_error(GL_INVALID_OPERATION,
                                   fmt::format("Texture type '{}' currently not supported", texture_type));
            }
        }

        // Tell OpenGL how to handle texture wrapping
        // ------------------------------------------
        void texture_wrap(const unsigned int& s_wrap,
                          const unsigned int& t_wrap,
                          const unsigned int& unit = GL_TEXTURE0) {
            bind(unit);
            glTexParameteri(texture_type, GL_TEXTURE_WRAP_S, s_wrap);
            glTexParameteri(texture_type, GL_TEXTURE_WRAP_T, t_wrap);
        }

        // Tell OpenGL how to handle texture minifying and magnifying
        // ----------------------------------------------------------
        void texture_filter(const unsigned int& min_filter,
                            const unsigned int& mag_filter,
                            const unsigned int& unit = GL_TEXTURE0) {
            bind(unit);
            glTexParameteri(texture_type, GL_TEXTURE_MIN_FILTER, min_filter);
            glTexParameteri(texture_type, GL_TEXTURE_MAG_FILTER, mag_filter);
        }

        // Allow this texture  wrapper to be passed OpenGL functions
        // OpenGL functions expect an unsigned int
        // ---------------------------------------------------------
        operator unsigned int() const {
            return tex;
        }

    private:
        unsigned int tex;
        TextureType texture_type;
        int width, height, channels;
        std::vector<unsigned char> texture_data;
    };
}  // namespace gl
}  // namespace utility


#endif  // UTILITY_OPENGL_UTILS_HPP
