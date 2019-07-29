#ifndef UTILITY_MESH_HPP
#define UTILITY_MESH_HPP

#include <cstddef>  // for offsetof
#include <string>
#include <vector>

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

#include "utility/opengl_utils.hpp"

namespace utility {
namespace mesh {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex;
    };
    struct Mesh {
        Mesh(const std::vector<Vertex>& vertices,
             const std::vector<unsigned int>& indices,
             const std::vector<utility::gl::texture>& textures)
            : vertices(vertices), indices(indices), textures(textures) {
            setup_mesh();
        }
        Mesh(std::vector<Vertex>&& vertices,
             std::vector<unsigned int>&& indices,
             std::vector<utility::gl::texture>&& textures)
            : vertices(vertices), indices(indices), textures(textures) {
            setup_mesh();
        }

        void render(utility::gl::shader_program& program) {
            int diffuse_count  = 0;
            int specular_count = 0;

            program.use();

            for (int i = 0; i < textures.size(); ++i) {
                std::string texture_uniform;
                switch (textures[i].style()) {
                    case utility::gl::TextureStyle::TEXTURE_DIFFUSE:
                        texture_uniform = fmt::format("material.diffuse[{}]", diffuse_count++);
                        break;
                    case utility::gl::TextureStyle::TEXTURE_SPECULAR:
                        texture_uniform = fmt::format("material.specular[{}]", specular_count++);
                        break;
                    default:
                        utility::gl::throw_gl_error(GL_INVALID_ENUM,
                                                    fmt::format("Invalid texture style '{}'", textures[i].style()));
                }
                textures[i].bind(GL_TEXTURE0 + i);
                program.set_uniform(texture_uniform, i);
            }

            program.set_uniform("material.diffuse_count", diffuse_count);
            program.set_uniform("material.specular_count", specular_count);

            // Render the mesh
            VAO.bind();
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            VAO.unbind();
        }

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<utility::gl::texture> textures;

    private:
        utility::gl::vertex_array VAO;
        utility::gl::vertex_buffer VBO;
        utility::gl::element_buffer EBO;

        void setup_mesh() {
            // Bind the vertex array
            VAO.bind();

            // Bind the vertex buffer and copy vertices to the device
            VBO.bind();
            VBO.copy_data(vertices, GL_STATIC_DRAW);

            // Bind the element buffer and copy indices to the device
            EBO.bind();
            EBO.copy_data(indices, GL_STATIC_DRAW);

            // Set up vertex attributes
            VAO.add_vertex_attrib<float>(0, 3, sizeof(Vertex), GL_FLOAT, false, offsetof(Vertex, position));
            VAO.add_vertex_attrib<float>(1, 3, sizeof(Vertex), GL_FLOAT, false, offsetof(Vertex, normal));
            VAO.add_vertex_attrib<float>(2, 2, sizeof(Vertex), GL_FLOAT, false, offsetof(Vertex, tex));

            // Unbind the vertex array, vertex buffer, and element buffer
            VAO.unbind();
            VBO.unbind();
            EBO.unbind();
        }
    };
}  // namespace mesh
}  // namespace utility


#endif  // UTILITY_MESH_HPP
