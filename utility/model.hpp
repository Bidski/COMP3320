#ifndef UTILITY_MODEL_HPP
#define UTILITY_MODEL_HPP

#include <string>
#include <vector>

// For model loading
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

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

#include "utility/mesh.hpp"
#include "utility/opengl_utils.hpp"

namespace utility {
namespace model {
    struct Model {
        Model(const std::string& model) {
            load_model(model);
        }

        void render(utility::gl::shader_program& program) {
            for (auto& mesh : meshes) {
                mesh.render(program);
            }
        }

    private:
        void load_model(const std::string& model) {
            Assimp::Importer importer;
            const aiScene* scene =
                importer.ReadFile(model, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                throw std::runtime_error(
                    fmt::format("Failed to load model '{}': {}", model, importer.GetErrorString()));
                return;
            }

            // Store parent directory of the model
            directory = model.substr(0, model.find_last_of('/'));

            process_node(scene->mRootNode, scene);
        }
        void process_node(const aiNode* node, const aiScene* scene) {
            // Process all the node's meshes (if any)
            for (size_t i = 0; i < node->mNumMeshes; ++i) {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(process_mesh(mesh, scene));
            }
            // Now process the nodes children (if any)
            for (size_t i = 0; i < node->mNumChildren; ++i) {
                process_node(node->mChildren[i], scene);
            }
        }
        utility::mesh::Mesh process_mesh(const aiMesh* mesh, const aiScene* scene) {
            std::vector<utility::mesh::Vertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<utility::gl::texture> textures;

            for (size_t i = 0; i < mesh->mNumVertices; ++i) {
                // process vertex positions, normals and texture coordinates
                utility::mesh::Vertex vertex = {
                    glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z),
                    glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
                    glm::vec2(0.0f)};

                // Check to see if the mesh contains any textures
                if (mesh->mTextureCoords[0]) {
                    vertex.tex = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
                }

                vertices.push_back(vertex);
            }
            // process indices
            for (size_t i = 0; i < mesh->mNumFaces; ++i) {
                aiFace face = mesh->mFaces[i];
                for (size_t j = 0; j < face.mNumIndices; ++j) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            // process material
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

                // Load diffuse maps
                std::vector<utility::gl::texture> diffuse_maps(
                    load_textures(material, aiTextureType_DIFFUSE, utility::gl::TextureStyle::TEXTURE_DIFFUSE));
                textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

                // Load specular maps
                std::vector<utility::gl::texture> specular_maps(
                    load_textures(material, aiTextureType_SPECULAR, utility::gl::TextureStyle::TEXTURE_SPECULAR));
                textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
            }

            return utility::mesh::Mesh(vertices, indices, textures);
        }

        std::vector<utility::gl::texture> load_textures(const aiMaterial* material,
                                                        const aiTextureType& type,
                                                        const utility::gl::TextureStyle& texture_style) {
            std::vector<utility::gl::texture> textures;
            for (size_t i = 0; i < material->GetTextureCount(type); ++i) {
                aiString str;
                material->GetTexture(type, i, &str);
                for (const auto& texture : loaded_textures) {
                    if (texture.path().compare(str.C_Str()) == 0) {
                        textures.push_back(texture);
                        continue;
                    }
                }
                utility::gl::texture texture(
                    fmt::format("{}/{}", directory, str.C_Str()), utility::gl::TextureType::TEXTURE_2D, texture_style);
                texture.bind();
                texture.generate(0);
                texture.generate_mipmap();
                texture.texture_wrap(GL_REPEAT, GL_REPEAT);
                texture.texture_filter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
                texture.unbind();
                textures.emplace_back(std::move(texture));
                loaded_textures.push_back(textures.back());
            }
            return textures;
        }

        std::vector<utility::mesh::Mesh> meshes;
        std::vector<utility::gl::texture> loaded_textures;
        std::string directory;
    };
}  // namespace model
}  // namespace utility


#endif  // UTILITY_MODEL_HPP
