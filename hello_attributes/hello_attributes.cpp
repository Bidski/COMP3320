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

#include "utility/opengl_utils.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void render(GLFWwindow* window);

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
    std::array<float, 30> vertices = {
        // positions         //colours
         0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top right
         0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, // top left
         0.0f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f  // center
    };
    // clang-format on

    // define indices
    // ---------------
    // clang-format off
    std::array<unsigned int, 12> indices = {
        1, 2, 4, // bottom
        0, 1, 4, // right
        0, 3, 4, // top
        2, 3, 4  // left
    };
    // clang-format on

    // load, compile, and link the vertex and fragment shaders
    // -------------------------------------------------------
    utility::gl::shader_program program;
    program.add_shader("hello_attributes.vert", GL_VERTEX_SHADER);
    program.add_shader("hello_attributes.frag", GL_FRAGMENT_SHADER);
    program.link();

    // create a vertex buffer object
    // -----------------------------
    utility::gl::vertex_buffer VBO;

    // create a vertex array object
    // -----------------------------
    utility::gl::vertex_array VAO;

    // create an element buffer object
    // -------------------------------
    utility::gl::element_buffer EBO;

    // bind the vertex array object
    // ----------------------------
    VAO.bind();

    // copy vertex data to GPU
    // -----------------------
    VBO.copy_data<30>(vertices, GL_STATIC_DRAW);

    // copy index data to GPU
    // ----------------------
    EBO.copy_data<12>(indices, GL_STATIC_DRAW);

    // define vertex attributes
    // ------------------------
    VAO.add_vertex_attrib<float>(0, 3, 6, GL_FLOAT, false, 0);
    VAO.add_vertex_attrib<float>(1, 3, 6, GL_FLOAT, false, 3);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound
    // vertex buffer object so afterwards we can safely unbind
    VBO.unbind();

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens.
    // Modifying other VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs)
    // when it's not directly necessary.
    VAO.unbind();

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO
    EBO.unbind();

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
        program.use();
        // seeing as we only have a single VAO there's no need to bind it every time, but we'll
        // do so to keep things a bit more organized
        VAO.bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        // VAO.unbind(); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
