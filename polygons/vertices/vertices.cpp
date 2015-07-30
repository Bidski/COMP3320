#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <GL/glew.h>                // Include this first
#include <GLFW/glfw3.h>

int main(void)
{
    // Initialise GLFW.
    if (glfwInit() == GL_FALSE)
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;

        return(-1);
    }

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

    if (window == NULL)
    {
        std::cerr << "Failed to create window." << std::endl;

        glfwTerminate();
        return(-1);
    }

    // Make the new context the active context.
    glfwMakeContextCurrent(window);

    // Tell GLEW to use the new (experimental) stuff.
    glewExperimental = GL_TRUE;

    // Make sure the context was created.
    GLenum err = glGetError();

    if (err != GL_NO_ERROR)
    {
        std::cerr << "OpenGL Error: " << err << std::endl;
    }

    // Initialise GLEW.
    err = glewInit();

    if (err != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        std::cerr << "Error: " <<  err << " - " << glewGetErrorString(err) << std::endl;

        glfwTerminate();
        return(-1);
    }

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create a Vertex Buffer Object (VBO).
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Specify the vertices.
    float vertices[] = 
    {
         0.0f,  0.5f, // Vertex 1 (X, Y)
         0.5f, -0.5f, // Vertex 2 (X, Y)
        -0.5f, -0.5f  // Vertex 3 (X, Y)
    };

    // Copy vertex data into the VBO.
    // GL_STATIC_DRAW: Copy vertex data to graphics card once, then redraw many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create the vertex shader.
    std::ifstream vertexSourceFile("vertexShader.glsl");
    std::vector<char> vertexSource;

    vertexSourceFile.seekg(0, std::ios::end);   
    vertexSource.reserve(vertexSourceFile.tellg());
    vertexSourceFile.seekg(0, std::ios::beg);

    vertexSource.assign((std::istreambuf_iterator<char>(vertexSourceFile)), std::istreambuf_iterator<char>());

    vertexSourceFile.close();
    const char* vertexShaderSource = vertexSource.data();

    // Compile vertex shader.
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Ensure the vertex shader compiled correctly.
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
        // Get the length of the compile log.
        GLint logLength;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> buffer(logLength);
        glGetShaderInfoLog(vertexShader, logLength, NULL, buffer.data());
        std::cerr << "Vertex shader failed to compile." << std::endl;
        std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;
    }

    // Create the fragment shader.
    std::ifstream fragmentSourceFile("fragmentShader.glsl");
    std::vector<char> fragmentSource;

    fragmentSourceFile.seekg(0, std::ios::end);   
    fragmentSource.reserve(fragmentSourceFile.tellg());
    fragmentSourceFile.seekg(0, std::ios::beg);

    fragmentSource.assign((std::istreambuf_iterator<char>(fragmentSourceFile)), std::istreambuf_iterator<char>());

    fragmentSourceFile.close();
    const char* fragmentShaderSource = fragmentSource.data();

    // Compile fragment shader.
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Ensure the fragment shader compiled correctly.
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
        // Get the length of the compile log.
        GLint logLength;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> buffer(logLength);
        glGetShaderInfoLog(fragmentShader, logLength, NULL, buffer.data());
        std::cerr << "Vertex fragment failed to compile." << std::endl;
        std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;
    }

    // Create the shader program.
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Bind fragment shader output to the correct output buffer.
    glBindFragDataLocation(shaderProgram, 0, "outColour");

    // Link the shader program.
    glLinkProgram(shaderProgram);

    // Start using the shader program.
    glUseProgram(shaderProgram);

    // Tell OpenGL where to find the position attributes in the vertex data.
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(posAttrib);

    // Main event loop.
    while(glfwWindowShouldClose(window) == GL_FALSE)
    {
        // Check for the escape key.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Clear the screen to black.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw a triangle from the 3 vertices.
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for window events.
        glfwPollEvents();
    }

    // Cleanup.
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vao);

    glfwTerminate();

    return(0);
}

