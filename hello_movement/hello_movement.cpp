#include <array>
#include <iostream>
#include <memory>

// For python style string formatting
#include "fmt/format.h"

// For matrix and vector arithmetic
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// clang-format off
// Must include glad first
#include "glad/glad.h"
#include "GLFW/glfw3.h"
// clang-format on

#include "utility/opengl_utils.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window, const float& delta_time);
void render(GLFWwindow* window);

// settings
constexpr unsigned int SCREEN_WIDTH  = 800;
constexpr unsigned int SCREEN_HEIGHT = 600;

// The screens current aspect ratio. This is updated in framebuffer_size_callback
float aspect_ratio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);

// The current position of the camera. This is updated in process_input
glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);

// The camera forward and up vectors
constexpr glm::vec3 camera_forward = glm::vec3(0.0f, 0.0f, -1.0f);
constexpr glm::vec3 camera_up      = glm::vec3(0.0f, 1.0f, 0.0f);

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
void process_input(GLFWwindow* window, const float& delta_time) {
    const float camera_speed = 2.5f * delta_time;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_position += camera_speed * camera_forward;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_position -= camera_speed * camera_forward;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_position -= glm::normalize(glm::cross(camera_forward, camera_up)) * camera_speed;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_position += glm::normalize(glm::cross(camera_forward, camera_up)) * camera_speed;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
}

void render(GLFWwindow* window) {
    // define vertices
    // ---------------
    // clang-format off
    std::array<float, 240> vertices = {
        // positions         //colours         // texture coords
          0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // front face: top right
          0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // front face: bottom right
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // front face: bottom left
         -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // front face: top left
          0.0f,  0.0f,  0.5f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, // front face: center
          
          0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // back face: top right
          0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // back face: bottom right
         -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // back face: bottom left
         -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // back face: top left
          0.0f,  0.0f, -0.5f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, // back face: center
          
          0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top face: front right
          0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // top face: back right
         -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top face: back left
         -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top face: front left
          0.0f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, // top face: center
          
          0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // bottom face: front right
          0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom face: back right
         -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom face: back left
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom face: front left
          0.0f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, // bottom face: center
          
         -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // left face: front top
         -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // left face: back top
         -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // left face: back bottom
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // left face: front bottom
         -0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, // left face: center
          
          0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // right face: front top
          0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // right face: back top
          0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, // right face: back bottom
          0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // right face: front bottom
          0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.5f, // right face: center
    };
    // clang-format on

    // define indices
    // ---------------
    // clang-format off
    std::array<unsigned int, 72> indices = {
        1, 2, 4, // front bottom
        0, 1, 4, // front right
        0, 3, 4, // front top
        2, 3, 4, // front left
        
        6, 7, 9, // back bottom
        5, 6, 9, // back right
        5, 8, 9, // back top
        7, 8, 9, // back left
        
        11, 12, 14, // top back
        10, 11, 14, // top right
        10, 13, 14, // top front
        12, 13, 14, // top left
        
        16, 17, 19, // bottom back
        15, 16, 19, // bottom right
        15, 18, 19, // bottom front
        17, 18, 19, // bottom left
        
        21, 22, 24, // left back
        20, 21, 24, // left bottom
        20, 23, 24, // left front
        22, 23, 24, // left top
        
        26, 27, 29, // right back
        25, 26, 29, // right bottom
        25, 28, 29, // right front
        27, 28, 29, // right top
    };
    // clang-format on

    // define positions for cubes to appear in
    // ---------------------------------------
    // clang-format off
    std::array<glm::vec3, 10> cube_positions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };
    // clang-format on

    // load, compile, and link the vertex and fragment shaders
    // -------------------------------------------------------
    utility::gl::shader_program program;
    program.add_shader("shaders/hello_movement.vert", GL_VERTEX_SHADER);
    program.add_shader("shaders/hello_movement.frag", GL_FRAGMENT_SHADER);
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
    VBO.copy_data<240>(vertices, GL_STATIC_DRAW);

    // copy index data to GPU
    // ----------------------
    EBO.copy_data<72>(indices, GL_STATIC_DRAW);

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

    // make sure OpenGL will perform depth testing
    // -------------------------------------------
    glEnable(GL_DEPTH_TEST);

    // keep track of frame rendering times
    // -----------------------------------
    float delta_time = 0.0f;
    float last_frame = glfwGetTime();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        float current_frame = glfwGetTime();
        float delta_time    = current_frame - last_frame;
        float last_frame    = current_frame;
        // input
        // -----
        process_input(window, delta_time);

        // clear the screen and the depth buffer
        // -------------------------------------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind the textures to the corresponding texture units
        // ----------------------------------------------------
        wall_texture.bind(GL_TEXTURE0);
        face_texture.bind(GL_TEXTURE1);

        // render our triangles
        // --------------------
        program.use();

        // update the uniforms
        // -------------------
        float mix_ratio = std::sin(current_frame) * 0.5f + 0.5f;
        program.set_uniform("mixRatio", mix_ratio);

        // create the world to view transform
        glm::mat4 Hvw = glm::lookAt(camera_position, camera_position + camera_forward, camera_up);

        // create the view clip transform (perspective projection)
        glm::mat4 Hcv = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 1000.0f);

        program.set_uniform("Hvw", Hvw);
        program.set_uniform("Hcv", Hcv);

        VAO.bind();

        // draw each cube
        for (size_t i = 0; i < cube_positions.size(); ++i) {
            // create the model to world transform
            glm::mat4 Hwm = glm::translate(glm::mat4(1.0f), cube_positions[i]);
            Hwm = glm::rotate(Hwm, glm::radians(current_frame * 50.0f + i * 20.0f), glm::vec3(1.0f, 0.3f, 0.5f));
            program.set_uniform("Hwm", Hwm);

            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
