#ifndef UTILITY_MODEL_HPP
#define UTILITY_MODEL_HPP

#include <iostream>
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
                importer.ReadFile(model,
                                  aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices
                                      | aiProcess_GenSmoothNormals);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
                throw std::runtime_error(
                    fmt::format("Failed to load model '{}': {}", model, importer.GetErrorString()));
                return;
            }

            // Store parent directory of the model
            directory = model.substr(0, model.find_last_of('/'));

            process_node(scene->mRootNode, scene);
        }

        void process_node(aiNode* node, const aiScene* scene) {
            // Process all the node's meshes (if any)
            meshes.reserve(meshes.size() + node->mNumMeshes);
            for (size_t i = 0; i < node->mNumMeshes; ++i) {
                // The node object only contains indices to index the actual objects in the scene.
                // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
                process_mesh(scene->mMeshes[node->mMeshes[i]], scene);
            }
            // Now process the nodes children (if any)
            for (size_t i = 0; i < node->mNumChildren; ++i) {
                process_node(node->mChildren[i], scene);
            }
        }

        void process_mesh(aiMesh* mesh, const aiScene* scene) {
            meshes.emplace_back();

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

                meshes.back().vertices.push_back(vertex);
            }

            // process indices
            for (size_t i = 0; i < mesh->mNumFaces; ++i) {
                aiFace face = mesh->mFaces[i];
                for (size_t j = 0; j < face.mNumIndices; ++j) {
                    meshes.back().indices.push_back(face.mIndices[j]);
                }
            }

            // process material
            if (mesh->mMaterialIndex >= 0) {
                aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

                // Load diffuse maps
                load_textures(material,
                              aiTextureType_DIFFUSE,
                              utility::gl::TextureStyle::TEXTURE_DIFFUSE,
                              meshes.back().textures);

                // Load specular maps
                load_textures(material,
                              aiTextureType_SPECULAR,
                              utility::gl::TextureStyle::TEXTURE_SPECULAR,
                              meshes.back().textures);
            }

            meshes.back().setup_mesh();
        }

        void load_textures(aiMaterial* material,
                           const aiTextureType& type,
                           const utility::gl::TextureStyle& texture_style,
                           std::vector<utility::gl::texture>& textures) {
            for (size_t i = 0; i < material->GetTextureCount(type); ++i) {
                aiString str;
                material->GetTexture(type, i, &str);
                textures.emplace_back(
                    fmt::format("{}/{}", directory, str.C_Str()), utility::gl::TextureType::TEXTURE_2D, texture_style);
                textures.back().bind(GL_TEXTURE0 + i);
                textures.back().generate(0);
                textures.back().generate_mipmap();
                textures.back().texture_wrap(GL_REPEAT, GL_REPEAT);
                textures.back().texture_filter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
            }
        }

        std::vector<utility::mesh::Mesh> meshes;
        std::string directory;
    };
}  // namespace model
}  // namespace utility


#endif  // UTILITY_MODEL_HPP
