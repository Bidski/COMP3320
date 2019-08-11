// https://ffainelli.github.io/openal-example/
#include <array>
#include <cstring>
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

#include "utility/openal_utils.hpp"

#include "utility/opengl_utils.hpp"

#include "utility/camera.hpp"

#include "utility/model.hpp"

struct PointLight {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float Kc;
    float Kl;
    float Kq;
};

void process_input(GLFWwindow* window,
                   const float& delta_time,
                   utility::camera::Camera& camera,
                   utility::al::OpenAL& sound_bite);
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
    GLFWwindow* window =
        glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "COMP3320 OpenGL Introduction: Audio Playback", NULL, NULL);
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
void process_input(GLFWwindow* window,
                   const float& delta_time,
                   utility::camera::Camera& camera,
                   utility::al::OpenAL& sound_bite) {
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
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        sound_bite.play_sound();
    }
}

void render(GLFWwindow* window, utility::camera::Camera& camera) {
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
    program.add_shader("shaders/openal.vert", GL_VERTEX_SHADER);
    program.add_shader("shaders/openal.frag", GL_FRAGMENT_SHADER);
    program.link();

    // make sure OpenGL will perform depth testing
    // -------------------------------------------
    glEnable(GL_DEPTH_TEST);

    // load nanosuit model
    // -------------------
    program.use();
    utility::model::Model nanosuit("models/nanosuit.obj");
    glm::vec3 nanosuit_position(0.0f, -1.75f, -2.0f);

    // keep track of frame rendering times
    // -----------------------------------
    float delta_time = 0.0f;
    float last_frame = glfwGetTime();

    // load up sound file
    // ------------------
    utility::al::OpenAL sound_bite(glm::vec3(0.0f, -1.75f, -2.0f));
    sound_bite.load_audio("audio/bugs_02.wav");

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // update sound listener and source positions
        // ------------------------------------------
        sound_bite.set_listener_position(camera.get_position());
        sound_bite.set_source_position(nanosuit_position);

        float current_frame = glfwGetTime();
        float delta_time    = current_frame - last_frame;
        float last_frame    = current_frame;
        // input
        // -----
        process_input(window, delta_time, camera, sound_bite);

        // clear the screen and the depth buffer
        // -------------------------------------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render our triangles
        // --------------------
        program.use();

        // update the uniforms
        // -------------------
        program.set_uniform("Hvw", camera.get_view_transform());
        program.set_uniform("Hcv", camera.get_clip_transform());
        glm::mat4 Hwm = glm::translate(glm::mat4(1.0f), nanosuit_position);
        Hwm           = glm::scale(Hwm, glm::vec3(0.2f, 0.2f, 0.2f));
        Hwm           = glm::rotate(Hwm, glm::radians(current_frame * 50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        program.set_uniform("Hwm", Hwm);

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

        // Render the nanosuit
        nanosuit.render(program);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
