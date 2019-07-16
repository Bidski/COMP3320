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
    std::array<float, 40> vertices = {
        // positions         //colours         // texture coords
         0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top right
         0.5f, -0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
         0.0f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f  // center
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
    program.add_shader("shaders/hello_textures.vert", GL_VERTEX_SHADER);
    program.add_shader("shaders/hello_textures.frag", GL_FRAGMENT_SHADER);
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

    // load textures
    // -------------
    utility::gl::texture wall_texture("textures/wall.jpg", GL_TEXTURE_2D);
    wall_texture.bind(GL_TEXTURE0);
    wall_texture.generate(0, GL_RGB);
    wall_texture.generate_mipmap();
    wall_texture.texture_wrap(GL_REPEAT, GL_REPEAT);
    wall_texture.texture_filter(GL_LINEAR, GL_LINEAR);

    utility::gl::texture face_texture("textures/awesomeface.png", GL_TEXTURE_2D);
    face_texture.bind(GL_TEXTURE1);
    face_texture.generate(0, GL_RGBA);
    face_texture.generate_mipmap();
    face_texture.texture_wrap(GL_REPEAT, GL_REPEAT);
    face_texture.texture_filter(GL_LINEAR, GL_LINEAR);

    // bind the vertex array object
    // ----------------------------
    VAO.bind();

    // copy vertex data to GPU
    // -----------------------
    VBO.copy_data<40>(vertices, GL_STATIC_DRAW);

    // copy index data to GPU
    // ----------------------
    EBO.copy_data<12>(indices, GL_STATIC_DRAW);

    // define vertex attributes
    // ------------------------
    VAO.add_vertex_attrib<float>(0, 3, 8, GL_FLOAT, false, 0);
    VAO.add_vertex_attrib<float>(1, 3, 8, GL_FLOAT, false, 3);
    VAO.add_vertex_attrib<float>(2, 2, 8, GL_FLOAT, false, 6);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound
    // vertex buffer object so afterwards we can safely unbind
    VBO.unbind();

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens.
    // Modifying other VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs)
    // when it's not directly necessary.
    VAO.unbind();

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO
    EBO.unbind();

    // set our texture uniforms
    program.use();
    program.set_uniform("ourTexture1", 0);
    program.set_uniform("ourTexture2", 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // clear the screen
        // ----------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind the textures to the corresponding texture units
        // ----------------------------------------------------
        wall_texture.bind(GL_TEXTURE0);
        face_texture.bind(GL_TEXTURE1);

        // render our triangles
        // --------------------
        program.use();

        // update the uniform
        // ------------------
        float mix_ratio = std::sin(glfwGetTime()) * 0.5f + 0.5f;
        program.set_uniform("mixRatio", mix_ratio);

        VAO.bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
