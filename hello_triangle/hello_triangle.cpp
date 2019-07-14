#include <array>
#include <iostream>
#include <memory>

// For python style string formatting
#include "fmt/format.h"

// clang-format off
// Must include glad first
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "utility/shader_utils.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void render(GLFWwindow* window);

namespace gl {
struct vertex_array {
    vertex_array() {
        glGenVertexArrays(1, &VAO);
    }
    ~vertex_array() {
        glDeleteVertexArrays(1, &VAO);
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
    void unbind() {
        glDeleteBuffers(1, &VBO);
    }

    operator unsigned int() const {
        return VBO;
    }

private:
    unsigned int VBO;
};
}  // namespace gl

// settings
constexpr unsigned int SCREEN_WIDTH  = 800;
constexpr unsigned int SCREEN_HEIGHT = 600;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "COMP3320 Hello OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cerr << fmt::format("Failed to create GLFW window with dimension {}x{}", SCREEN_WIDTH, SCREEN_HEIGHT)
                  << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    render(window);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void render(GLFWwindow* window) {
    // define vertices
    // ---------------
    // clang-format off
    std::array<float, 9> vertices = {
        -0.5f, -0.5f,  0.0f,
         0.5f, -0.5f,  0.0f,
         0.0f,  0.5f,  0.0f
    };
    // clang-format on

    // load and compile the vertex and fragment shaders
    // ------------------------------------------------
    unsigned int vertex_shader, fragment_shader;
    if (!utility::shader::compilerShader("hello_triangle.vert", GL_VERTEX_SHADER, vertex_shader)) {
        std::cerr << "Failed to compile vertex shader. Aborting." << std::endl;
        return;
    }
    if (!utility::shader::compilerShader("hello_triangle.frag", GL_FRAGMENT_SHADER, fragment_shader)) {
        std::cerr << "Failed to compile fragment shader. Aborting." << std::endl;
        return;
    }

    // link shaders into a program and use it
    // --------------------------------------
    unsigned int shader_program;
    if (!utility::shader::linkShaderProgram({vertex_shader, fragment_shader}, shader_program)) {
        std::cerr << "Failed to link shader program. Aborting." << std::endl;
        return;
    }

    // delete the shader objects, they aren't needed any more
    // ------------------------------------------------------
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // create a vertex buffer object
    // -----------------------------
    gl::vertex_buffer VBO;

    // create a vertex array object
    // -----------------------------
    gl::vertex_array VAO;

    // bind the vertex array object
    // ----------------------------
    glBindVertexArray(VAO);

    // bind the vertex buffer object
    // ----------------------------
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // copy vertex data to GPU
    // -----------------------
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    // define vertex attributes
    // ------------------------
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound
    // vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens.
    // Modifying other VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs)
    // when it's not directly necessary.
    glBindVertexArray(0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw our first triangle
        glUseProgram(shader_program);
        // seeing as we only have a single VAO there's no need to bind it every time, but we'll
        // do so to keep things a bit more organized
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
