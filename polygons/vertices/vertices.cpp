#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <GL/glew.h>                // Include this first
#include <GLFW/glfw3.h>

GLFWwindow* init(bool windowed, size_t width, size_t height);
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool loadShader(GLuint* shader, GLenum type, const std::string& file);
bool getShaderCompileStatus(GLuint shader);

int main(void)
{
    // Set error callback.
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = init(true, 800, 600);

    if (window == nullptr)
    {
        return(-1);
    }

    // Set keypress callback.
    glfwSetKeyCallback(window, key_callback);

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
    GLuint vertexShader;

    if (loadShader(&vertexShader, GL_VERTEX_SHADER, "vertexShader.glsl") == false)
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        glfwTerminate();

        return(-1);
    }

    // Create the fragment shader.
    GLuint fragmentShader;

    if (loadShader(&fragmentShader, GL_FRAGMENT_SHADER, "fragmentShader.glsl") == false)
    {
        glDeleteShader(vertexShader);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        glfwTerminate();

        return(-1);
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
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return(0);
}

GLFWwindow* init(bool windowed, size_t width, size_t height)
{
    // Initialise GLFW.
    if (glfwInit() == GL_FALSE)
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;

        return(nullptr);
    }

    // Set up OpenGL version,
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // Request 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Specify that we only accept contextes that support the new core functionality.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Window is not resizable.
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a window.
    GLFWwindow* window = nullptr;

    if (windowed == true)
    {
        window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
    }

    else
    {
        window = glfwCreateWindow(width, height, "OpenGL", glfwGetPrimaryMonitor(), nullptr);
    }

    if (window == nullptr)
    {
        std::cerr << "Failed to create window." << std::endl;

        glfwTerminate();
        return(nullptr);
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
        return(nullptr);
    }

    return(window);
}

// Define an error callback.
static void error_callback(int error, const char* description)
{
    std::cerr << "Error " << error << " occurred: " << description << std::endl;
}

// Define the key input callback.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

bool loadShader(GLuint* shader, GLenum type, const std::string& file)
{
    // Create the vertex shader.
    std::ifstream sourceStream(file);
    std::string source;

    sourceStream.seekg(0, std::ios::end);
    source.reserve(sourceStream.tellg());
    sourceStream.seekg(0, std::ios::beg);

    source.assign((std::istreambuf_iterator<char>(sourceStream)), std::istreambuf_iterator<char>());

    sourceStream.close();

    // Compile shader.
    const char* shaderSource = source.c_str();
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &shaderSource, nullptr);
    glCompileShader(*shader);

    return(getShaderCompileStatus(*shader));
}

bool getShaderCompileStatus(GLuint shader)
{
    //Get status
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if(status == GL_TRUE)
    {
        return(true);
    }

    else
    {
        // Get the length of the compile log.
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        // Get log.
        std::vector<char> buffer(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, buffer.data());
        std::cerr << "Vertex shader failed to compile." << std::endl;
        std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;

        return(false);
    }
}



