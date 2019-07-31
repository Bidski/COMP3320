#ifndef UTILITY_MESH_HPP
#define UTILITY_MESH_HPP

#include <cstddef>  // for offsetof
#include <iostream>
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
#pragma pack(push, 1)
    struct Vertex {
        Vertex(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& tex)
            : position(position), normal(normal), tex(tex) {}
        Vertex(glm::vec3&& position, glm::vec3&& normal, glm::vec2&& tex)
            : position(position), normal(normal), tex(tex) {}
        Vertex(const Vertex& vertex) : position(vertex.position), normal(vertex.normal), tex(vertex.tex) {}
        Vertex(Vertex&& vertex) noexcept
            : position(std::move(vertex.position)), normal(std::move(vertex.normal)), tex(std::move(vertex.tex)) {}
        Vertex& operator=(const Vertex& vertex) {
            position = vertex.position;
            normal   = vertex.normal;
            tex      = vertex.tex;
            return *this;
        }
        Vertex& operator=(Vertex&& vertex) {
            position = std::move(vertex.position);
            normal   = std::move(vertex.normal);
            tex      = std::move(vertex.tex);
            return *this;
        }
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex;
    };
    static_assert(sizeof(Vertex) == 32, "The compiler is adding padding to this struct, Bad compiler!");
#pragma pack(pop)
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
        ~Mesh() {
            vertices.clear();
            indices.clear();
            textures.clear();
        }
        Mesh(const Mesh& mesh)
            : vertices(mesh.vertices)
            , indices(mesh.indices)
            , textures(mesh.textures)
            , VAO(mesh.VAO)
            , VBO(mesh.VBO)
            , EBO(mesh.EBO) {}
        Mesh(Mesh&& mesh) noexcept
            : vertices(std::move(mesh.vertices))
            , indices(std::move(mesh.indices))
            , textures(std::move(mesh.textures))
            , VAO(std::move(mesh.VAO))
            , VBO(std::move(mesh.VBO))
            , EBO(std::move(mesh.EBO)) {}
        Mesh& operator=(const Mesh& mesh) {
            vertices = mesh.vertices;
            indices  = mesh.indices;
            textures = mesh.textures;
            VAO      = mesh.VAO;
            VBO      = mesh.VBO;
            EBO      = mesh.EBO;
            return *this;
        }
        Mesh& operator=(Mesh&& mesh) {
            vertices = std::move(mesh.vertices);
            indices  = std::move(mesh.indices);
            textures = std::move(mesh.textures);
            VAO      = std::move(mesh.VAO);
            VBO      = std::move(mesh.VBO);
            EBO      = std::move(mesh.EBO);
            return *this;
        }

        void render(utility::gl::shader_program& program) {
            int diffuse_count  = 0;
            int specular_count = 0;

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
