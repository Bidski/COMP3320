#include <iostream>

#include <GL/glew.h>                // Include this first
#include <GLFW/glfw3.h>

int main()
{
    glfwInit();

    // Set up OpenGL version,
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // Specify that we only accept contextes that support the new core functionality.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Window is not resizable.
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a window.
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed

    // Work in fullscreen.
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen

    // Make the new context the active context.
    glfwMakeContextCurrent(window);

    // Initialise GLEW.
    glewExperimental = GL_TRUE;
    glewInit();

    // Main event loop.
    while(!glfwWindowShouldClose(window))
    {
        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for window events.
        glfwPollEvents();

        // Check for the escape key.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        std::cout << vertexBuffer << std::endl;
    }

    glfwTerminate();
}

