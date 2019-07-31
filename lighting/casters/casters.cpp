#include <array>
#include <functional>
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

#include "utility/camera.hpp"

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float Kc;
    float Kl;
    float Kq;
};

void process_input(GLFWwindow* window, const float& delta_time, utility::camera::Camera& camera);
void render(GLFWwindow* window, utility::camera::Camera& camera);

// Initial width and height of the window
static constexpr int SCREEN_WIDTH  = 800;
static constexpr int SCREEN_HEIGHT = 600;

// Distances to the near and the far plane. Used for the camera to clip space transform.
static constexpr float NEAR_PLANE = 0.1f;
static constexpr float FAR_PLANE  = 1000.0f;

int main() {
    // create our camera objects
    // -------------------------
    utility::camera::Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT, NEAR_PLANE, FAR_PLANE);

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(
        SCREEN_WIDTH, SCREEN_HEIGHT, "COMP3320 OpenGL Introduction: Keyboard and Mouse Input", NULL, NULL);
    if (window == NULL) {
        std::cerr << fmt::format("Failed to create GLFW window with dimension {}x{}", SCREEN_WIDTH, SCREEN_HEIGHT)
                  << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,
                                   CCallbackWrapper(GLFWframebuffersizefun, utility::camera::Camera)(
                                       std::bind(&utility::camera::Camera::framebuffer_size_callback,
                                                 &camera,
                                                 std::placeholders::_1,
                                                 std::placeholders::_2,
                                                 std::placeholders::_3)));

    // get glfw to capture and hide the mouse pointer
    // ----------------------------------------------
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(
        window,
        CCallbackWrapper(GLFWcursorposfun, utility::camera::Camera)(std::bind(&utility::camera::Camera::mouse_callback,
                                                                              &camera,
                                                                              std::placeholders::_1,
                                                                              std::placeholders::_2,
                                                                              std::placeholders::_3)));

    // get glfw to capture mouse scrolling
    // -----------------------------------
    glfwSetScrollCallback(
        window,
        CCallbackWrapper(GLFWscrollfun, utility::camera::Camera)(std::bind(&utility::camera::Camera::scroll_callback,
                                                                           &camera,
                                                                           std::placeholders::_1,
                                                                           std::placeholders::_2,
                                                                           std::placeholders::_3)));

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    render(window, camera);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void process_input(GLFWwindow* window, const float& delta_time, utility::camera::Camera& camera) {
    camera.set_movement_sensitivity(0.005f * delta_time);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.move_forward();
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.move_backward();
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.move_left();
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.move_right();
    }
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.move_up();
    }
    else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        camera.move_down();
    }
}

void render(GLFWwindow* window, utility::camera::Camera& camera) {
    // define vertices
    // ---------------
    // clang-format off
    std::array<float, 330> vertices = {
        // positions          //colours         //normals            // texture coords
          0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // front face: top right
          0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // front face: bottom right
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // front face: bottom left
         -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // front face: top left
          0.0f,  0.0f,  0.5f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f, 0.5f, 0.5f, // front face: center

          0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // back face: top right
          0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // back face: bottom right
         -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // back face: bottom left
         -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // back face: top left
          0.0f,  0.0f, -0.5f, 1.0f, 1.0f, 1.0f,  0.0f,  0.0f, -1.0f, 0.5f, 0.5f, // back face: center

          0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top face: front right
          0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // top face: back right
         -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // top face: back left
         -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top face: front left
          0.0f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f, 0.5f, 0.5f, // top face: center

          0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // bottom face: front right
          0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom face: back right
         -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom face: back left
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // bottom face: front left
          0.0f, -0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  0.0f, -1.0f,  0.0f, 0.5f, 0.5f, // bottom face: center

         -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // left face: front top
         -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // left face: back top
         -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // left face: back bottom
         -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // left face: front bottom
         -0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f, 0.5f, 0.5f, // left face: center

          0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // right face: front top
          0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // right face: back top
          0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // right face: back bottom
          0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // right face: front bottom
          0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f, 0.5f, 0.5f, // right face: center
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

    // positions of the point lights
    std::array<PointLight, 4> point_lights = {
        PointLight{
            glm::vec3(0.7f, 0.2f, 2.0f), glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f},
        PointLight{
            glm::vec3(2.3f, -3.3f, -4.0f), glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f},
        PointLight{
            glm::vec3(-4.0f, 2.0f, -12.0f), glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f},
        PointLight{
            glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.05f), glm::vec3(0.8f), glm::vec3(1.0f), 1.0f, 0.09f, 0.032f}};

    // load, compile, and link the vertex and fragment shaders
    // -------------------------------------------------------
    utility::gl::shader_program program;
    program.add_shader("shaders/casters.vert", GL_VERTEX_SHADER);
    program.add_shader("shaders/casters.frag", GL_FRAGMENT_SHADER);
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
    utility::gl::texture diffuse_texture("textures/container_diffuse.png", GL_TEXTURE_2D);
    diffuse_texture.bind(GL_TEXTURE0);
    diffuse_texture.generate(0);
    diffuse_texture.generate_mipmap();
    diffuse_texture.texture_wrap(GL_REPEAT, GL_REPEAT);
    diffuse_texture.texture_filter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    utility::gl::texture specular_texture("textures/container_specular.png", GL_TEXTURE_2D);
    specular_texture.bind(GL_TEXTURE1);
    specular_texture.generate(0);
    specular_texture.generate_mipmap();
    specular_texture.texture_wrap(GL_REPEAT, GL_REPEAT);
    specular_texture.texture_filter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // bind the vertex array object
    // ----------------------------
    VAO.bind();

    // copy vertex data to GPU
    // -----------------------
    VBO.copy_data<330>(vertices, GL_STATIC_DRAW);

    // copy index data to GPU
    // ----------------------
    EBO.copy_data<72>(indices, GL_STATIC_DRAW);

    // define vertex attributes
    // ------------------------
    VAO.add_vertex_attrib<float>(0, 3, 11, GL_FLOAT, false, 0);
    VAO.add_vertex_attrib<float>(1, 3, 11, GL_FLOAT, false, 3);
    VAO.add_vertex_attrib<float>(2, 3, 11, GL_FLOAT, false, 6);
    VAO.add_vertex_attrib<float>(3, 2, 11, GL_FLOAT, false, 9);

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
    program.set_uniform("material.diffuse", 0);
    program.set_uniform("material.specular", 1);

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
        process_input(window, delta_time, camera);

        // clear the screen and the depth buffer
        // -------------------------------------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind diffuse texture
        // --------------------
        diffuse_texture.bind(GL_TEXTURE0);
        specular_texture.bind(GL_TEXTURE1);

        // render our triangles
        // --------------------
        program.use();

        // update the uniforms
        // -------------------
        program.set_uniform("Hvw", camera.get_view_transform());
        program.set_uniform("Hcv", camera.get_clip_transform());

        program.set_uniform("material.shininess", 32.0f);
        program.set_uniform("viewPosition", camera.get_position());

        program.set_uniform("sun.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        program.set_uniform("sun.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        program.set_uniform("sun.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        program.set_uniform("sun.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        for (size_t i = 0; i < 4; ++i) {
            program.set_uniform(fmt::format("lights[{}].position", i), point_lights[i].position);
            program.set_uniform(fmt::format("lights[{}].ambient", i), point_lights[i].ambient);
            program.set_uniform(fmt::format("lights[{}].diffuse", i), point_lights[i].diffuse);
            program.set_uniform(fmt::format("lights[{}].specular", i), point_lights[i].specular);
            program.set_uniform(fmt::format("lights[{}].Kc", i), point_lights[i].Kc);
            program.set_uniform(fmt::format("lights[{}].Kl", i), point_lights[i].Kl);
            program.set_uniform(fmt::format("lights[{}].Kq", i), point_lights[i].Kq);
        }

        program.set_uniform("lamp.position", camera.get_position());
        program.set_uniform("lamp.direction", camera.get_view_direction());
        program.set_uniform("lamp.ambient", glm::vec3(0.0f));
        program.set_uniform("lamp.diffuse", glm::vec3(1.0f));
        program.set_uniform("lamp.specular", glm::vec3(1.0f));
        program.set_uniform("lamp.phi", std::cos(glm::radians(12.5f)));
        program.set_uniform("lamp.gamma", std::cos(glm::radians(15.0f)));
        program.set_uniform("lamp.Kc", 1.000f);
        program.set_uniform("lamp.Kl", 0.090f);
        program.set_uniform("lamp.Kq", 0.032f);

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
