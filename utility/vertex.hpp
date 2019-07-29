#ifndef UTILITY_VERTEX_HPP
#define UTILITY_VERTEX_HPP

// For matrix and vector arithmetic
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace utility {
namespace vertex {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 tex;
    };
}  // namespace vertex
}  // namespace utility


#endif  // UTILITY_VERTEX_HPP
