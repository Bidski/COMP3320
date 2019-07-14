#ifndef UTILITY_SHADER_UTILS_HPP
#define UTILITY_SHADER_UTILS_HPP

#include <fstream>
#include <sstream>
#include <string>

// For python style string formatting
#include "fmt/format.h"

// clang-format off
// Must include glad first
#include "glad/glad.h"
// clang-format on

namespace utility {
namespace shader {
    inline bool compilerShader(const std::string& shader_file,
                               const unsigned int& shader_type,
                               unsigned int& shader_id) {
        // Load shader source code from the specified file
        // See: http://stackoverflow.com/a/116228
        std::ifstream data(shader_file, std::ios::in);
        if (!data.good()) {
            std::cerr << fmt::format("Failed to open shader file '{}'", shader_file) << std::endl;
        }
        std::stringstream stream;
        stream << data.rdbuf();
        std::string vertex_shader_source(stream.str());

        shader_id = glCreateShader(shader_type);

        // glShaderSource expects an array of strings
        const char* shader_src = vertex_shader_source.c_str();
        glShaderSource(shader_id, 1, &shader_src, nullptr);
        glCompileShader(shader_id);

        // Check for compile errors
        int success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

        if (!success) {
            std::string shader_type_str;
            switch (shader_type) {
                case GL_VERTEX_SHADER: shader_type_str = "GL_VERTEX_SHADER"; break;
                case GL_FRAGMENT_SHADER: shader_type_str = "GL_FRAGMENT_SHADER"; break;
                default: shader_type_str = "UNKNOWN_SHADER"; break;
            }
            char info_log[512];
            glGetShaderInfoLog(shader_id, 512, nullptr, info_log);
            std::cerr << fmt::format(
                "ERROR:\nShader Compilation Failed:\nShader Type:{}\nLog:\n{}", shader_type_str, info_log)
                      << std::endl;
            return false;
        }
        else {
            return true;
        }
    }

    inline bool linkShaderProgram(const std::initializer_list<unsigned int>& shader_ids, unsigned int& program_id) {
        // Create a shader program
        program_id = glCreateProgram();

        // Attach each of the provided shaders to the progrm
        for (const auto& shader_id : shader_ids) {
            glAttachShader(program_id, shader_id);
        }

        // Link all of the shaders together
        glLinkProgram(program_id);

        // Check for linking errors
        int success;
        glGetProgramiv(program_id, GL_LINK_STATUS, &success);

        if (!success) {
            char info_log[512];
            glGetProgramInfoLog(program_id, 512, nullptr, info_log);
            std::cerr << fmt::format("ERROR:\nProgram Linking Failed:\nLog:\n{}", info_log) << std::endl;
            return false;
        }
        else {
            return true;
        }
    }
}  // namespace shader
}  // namespace utility


#endif  // UTILITY_SHADER_UTILS_HPP
