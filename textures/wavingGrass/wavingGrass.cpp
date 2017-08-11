#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

#include <GL/glew.h>                // Include this first
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

GLFWwindow* init(bool windowed, size_t width, size_t height);
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool loadShader(GLuint* shader, GLenum type, const std::string& file);
GLuint loadShaderProgram(const std::vector<std::pair<std::string, GLuint>>& shaders);
bool getShaderCompileStatus(GLuint shader);
void generateTerrain(float size, GLuint width, GLuint depth, float vertices[], GLuint elements[]);

glm::vec3 cameraPosition;
glm::vec3 cameraLookAt;
glm::vec3 cameraUp;

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

    // Create Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object (VBO).
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Specify the vertices.
    float vertices[2 * 2 * 16];
    GLuint elements[2 * 2 * 6];
    generateTerrain(0.05f, 2, 2, vertices, elements);

    // Copy vertex data into the VBO.
    // GL_STATIC_DRAW: Copy vertex data to graphics card once, then redraw many times.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an Element Buffer Object (EBO).
    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Create the shader program.
    GLuint groundShaderProgram = loadShaderProgram({{"ground-vertex.glsl", GL_VERTEX_SHADER},
                                                    {"fragment.glsl", GL_FRAGMENT_SHADER}});

    if (groundShaderProgram < 0)
    {
        std::cerr << "Failed to link shader program" << std::endl;
    }

    // Start using the shader program.
    glUseProgram(groundShaderProgram);

    // Tell OpenGL where to find the position attributes in the vertex data.
    // There are 4 elements for each vector, the first two are the (X, Y) coordinates.
    GLint position = glGetAttribLocation(groundShaderProgram, "position");
    glEnableVertexAttribArray(position);
    glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    // Create perspective matrix
    GLint groundPerspective = glGetUniformLocation(groundShaderProgram, "perspectiveMatrix");
    GLint groundCamera      = glGetUniformLocation(groundShaderProgram, "cameraMatrix");
    GLint groundModel       = glGetUniformLocation(groundShaderProgram, "modelMatrix");

    glm::mat4 perspectiveMatrix = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(groundPerspective, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

    glUseProgram(0);

    // Create the shader program.
    GLuint grassShaderProgram = loadShaderProgram({{"pass-through-vertex.glsl", GL_VERTEX_SHADER},
                                                   {"grass.glsl", GL_GEOMETRY_SHADER},
                                                   {"fragment.glsl", GL_FRAGMENT_SHADER}});

    // Start using the shader program.
    glUseProgram(grassShaderProgram);

    // Create perspective matrix
    GLint grassPerspective = glGetUniformLocation(grassShaderProgram, "perspectiveMatrix");
    GLint grassCamera      = glGetUniformLocation(grassShaderProgram, "cameraMatrix");
    GLint grassModel       = glGetUniformLocation(grassShaderProgram, "modelMatrix");
    glUniformMatrix4fv(grassPerspective, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

    glUseProgram(0);

    cameraPosition           = glm::vec3(0.0f, 0.0f,  0.0f);
    cameraLookAt             = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp                 = glm::vec3(0.0f, 0.0f,  1.0f);
    glm::mat4 modelTransform = glm::translate(glm::mat4(), glm::vec3(0, -0.15f, -0.5f));

    // Main event loop.
    while(glfwWindowShouldClose(window) == GL_FALSE)
    {
        glm::mat4 cameraTransform = glm::lookAt(cameraPosition, cameraLookAt, cameraUp);

        // Clear the screen to black.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(groundShaderProgram);
        glUniformMatrix4fv(groundCamera, 1, GL_FALSE, glm::value_ptr(cameraTransform));
        glUniformMatrix4fv(groundModel,  1, GL_FALSE, glm::value_ptr(modelTransform));
        glDrawElements(GL_TRIANGLES, 2 * 2 * 6, GL_UNSIGNED_INT, 0);

        glUseProgram(grassShaderProgram);
        glUniformMatrix4fv(grassCamera, 1, GL_FALSE, glm::value_ptr(cameraTransform));
        glUniformMatrix4fv(grassModel,  1, GL_FALSE, glm::value_ptr(modelTransform));
        glDrawElements(GL_TRIANGLES, 2 * 2 * 6, GL_UNSIGNED_INT, 0);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for window events.
        glfwPollEvents();
    }

    // Cleanup.
    glDeleteProgram(groundShaderProgram);
    glDeleteProgram(grassShaderProgram);
    glDeleteBuffers(1, &ebo);
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
        window = glfwCreateWindow(width, height, "OpenGL Grass", nullptr, nullptr);
    }

    else
    {
        window = glfwCreateWindow(width, height, "OpenGL Grass", glfwGetPrimaryMonitor(), nullptr);
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
    else if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        cameraPosition.z += 0.05f;
        cameraLookAt = cameraPosition + glm::vec3(0.0f, 0.0f, -1.0f);
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        cameraLookAt = glm::rotate(cameraLookAt, -0.05f, cameraUp);
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        cameraPosition.z -= 0.05f;
        cameraLookAt = cameraPosition + glm::vec3(0.0f, 0.0f, -1.0f);
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        cameraLookAt = glm::rotate(cameraLookAt, 0.05f, cameraUp);
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

GLuint loadShaderProgram(const std::vector<std::pair<std::string, GLuint>>& shaders)
{
    GLuint shaderProgram = glCreateProgram();
    for (const auto& shader : shaders)
    {
        GLuint s;
        if (!loadShader(&s, shader.second, shader.first))
        {
            return(-1);
        }

        glAttachShader(shaderProgram, s);
    }

    // Bind fragment shader output to the correct output buffer.
    glBindFragDataLocation(shaderProgram, 0, "outColour");

    // Link the shader program.
    glLinkProgram(shaderProgram);

    if (!getShaderLinkStatus(shaderProgram))
    {

    }

    return shaderProgram;
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

bool getProgramLinkStatus(GLuint shader)
{
    //Get status
    GLint status;
    glGetShaderiv(shader, GL_LINK_STATUS, &status);

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
        glGetProgramInfoLog(shader, logLength, nullptr, buffer.data());
        std::cerr << "Vertex shader failed to compile." << std::endl;
        std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;

        return(false);
    }
}

void generateTerrain(float size, GLuint width, GLuint depth, float vertices[], GLuint elements[])
{
    // The vertices of a triangle.
    float triangle[16] =
    {
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    for (GLuint i = 0; i < width; i++)
    {
        for (GLuint j = 0; j < depth; j++)
        {
            triangle[ 4] = (i + 1) * size;
            triangle[10] = (j + 1) * size;
            triangle[12] = (i + 1) * size;
            triangle[14] = (j + 1) * size;

            std::memcpy(vertices + (16 * width * depth) + (j * width) + i, triangle, 16 * sizeof(float));
        }
    }

    for (GLuint i = 0;  i < (width * depth); i++)
    {
        const GLuint base = i * 4;
        elements[(i * 6) + 0] = base + 0;
        elements[(i * 6) + 1] = base + 1;
        elements[(i * 6) + 2] = base + 2;
        elements[(i * 6) + 3] = base + 1;
        elements[(i * 6) + 4] = base + 3;
        elements[(i * 6) + 5] = base + 2;
    }
}
