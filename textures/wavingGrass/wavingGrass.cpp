#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <GL/glew.h>  // Include this first
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

GLFWwindow* init(bool windowed, size_t width, size_t height);
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool loadShader(GLuint* shader, GLenum type, const std::string& file);
GLuint loadShaderProgram(const std::vector<std::pair<std::string, GLuint>>& shaders);
bool getShaderCompileStatus(GLuint shader);
bool getProgramLinkStatus(GLuint program);
void generateTerrain(float size, GLuint width, GLuint depth, float vertices[], GLuint elements[]);
glm::mat4x4 lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up);

const GLuint WIDTH = 6;
const GLuint DEPTH = 10;

const glm::vec3 cameraRotatedUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 cameraPosition;
glm::vec3 cameraLookAt;
glm::vec3 cameraRight;
glm::vec3 cameraUp;

int main(void) {
    // Set error callback.
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = init(true, 800, 600);

    if (window == nullptr) {
        return -1;
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
    float vertices[WIDTH * DEPTH * 16];
    GLuint elements[WIDTH * DEPTH * 6];
    generateTerrain(0.05f, WIDTH, DEPTH, vertices, elements);

    // Copy vertex data into the VBO.
    // GL_STATIC_DRAW: Copy vertex data to graphics card once, then redraw many
    // times.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an Element Buffer Object (EBO).
    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Set up basic screen properties
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    // Create the shader program.
    GLuint groundShaderProgram =
        loadShaderProgram({{"ground-vertex.glsl", GL_VERTEX_SHADER}, {"fragment.glsl", GL_FRAGMENT_SHADER}});

    if (groundShaderProgram < 0) {
        std::cerr << "Failed to link shader program" << std::endl;
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glfwTerminate();
        return -1;
    }

    // Start using the shader program.
    glUseProgram(groundShaderProgram);

    // Tell OpenGL where to find the position attributes in the vertex data.
    // There are 4 elements for each vector, the first two are the (X, Y)
    // coordinates.
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

    if (grassShaderProgram < 0) {
        std::cerr << "Failed to link shader program" << std::endl;
        glDeleteProgram(grassShaderProgram);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glfwTerminate();
        return -1;
    }

    // Start using the shader program.
    glUseProgram(grassShaderProgram);

    // Create perspective matrix
    GLint grassPerspective = glGetUniformLocation(grassShaderProgram, "perspectiveMatrix");
    GLint grassCamera      = glGetUniformLocation(grassShaderProgram, "cameraMatrix");
    GLint grassModel       = glGetUniformLocation(grassShaderProgram, "modelMatrix");
    glUniformMatrix4fv(grassPerspective, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));

    glUseProgram(0);

    cameraPosition           = glm::vec3(0.0f, 0.0f, 0.0f);
    cameraLookAt             = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp                 = glm::vec3(0.0f, 1.0f, 0.0f);
    cameraRight              = glm::cross(cameraUp, cameraLookAt);
    glm::mat4 modelTransform = glm::translate(glm::mat4(), glm::vec3(0, -0.15f, -0.5f));

    // Main event loop.
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // Change camera position
        glm::mat4 cameraTransform = lookAt(cameraPosition, cameraLookAt, cameraUp);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(groundShaderProgram);
        glUniformMatrix4fv(groundPerspective, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
        glUniformMatrix4fv(groundCamera, 1, GL_FALSE, glm::value_ptr(cameraTransform));
        glUniformMatrix4fv(groundModel, 1, GL_FALSE, glm::value_ptr(modelTransform));
        glDrawElements(GL_TRIANGLES, WIDTH * DEPTH * 6, GL_UNSIGNED_INT, 0);

        glUseProgram(grassShaderProgram);
        glUniformMatrix4fv(grassPerspective, 1, GL_FALSE, glm::value_ptr(perspectiveMatrix));
        glUniformMatrix4fv(grassCamera, 1, GL_FALSE, glm::value_ptr(cameraTransform));
        glUniformMatrix4fv(grassModel, 1, GL_FALSE, glm::value_ptr(modelTransform));
        glDrawElements(GL_TRIANGLES, WIDTH * DEPTH * 6, GL_UNSIGNED_INT, 0);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for window events.
        glfwPollEvents();
    }

    // Cleanup.
    glUseProgram(0);
    glDeleteProgram(groundShaderProgram);
    glDeleteProgram(grassShaderProgram);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return 0;
}

GLFWwindow* init(bool windowed, size_t width, size_t height) {
    // Initialise GLFW.
    if (glfwInit() == GL_FALSE) {
        std::cerr << "Failed to initialize GLFW" << std::endl;

        return nullptr;
    }

    // Set up OpenGL version,
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // Request 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Specify that we only accept contextes that support the new core
    // functionality.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Window is not resizable.
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a window.
    GLFWwindow* window = nullptr;

    if (windowed == true) {
        window = glfwCreateWindow(width, height, "OpenGL Grass", nullptr, nullptr);
    }

    else {
        window = glfwCreateWindow(width, height, "OpenGL Grass", glfwGetPrimaryMonitor(), nullptr);
    }

    if (window == nullptr) {
        std::cerr << "Failed to create window." << std::endl;

        glfwTerminate();
        return nullptr;
    }

    // Make the new context the active context.
    glfwMakeContextCurrent(window);

    // Tell GLEW to use the new (experimental) stuff.
    glewExperimental = GL_TRUE;

    // Make sure the context was created.
    GLenum err = glGetError();

    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << err << std::endl;
    }

    // Initialise GLEW.
    err = glewInit();

    if (err != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        std::cerr << "Error: " << err << " - " << glewGetErrorString(err) << std::endl;

        glfwTerminate();
        return (nullptr);
    }

    return window;
}

// Define an error callback.
static void error_callback(int error, const char* description) {
    std::cerr << "Error " << error << " occurred: " << description << std::endl;
}

// Define the key input callback.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        cameraPosition += glm::normalize(cameraLookAt) * 0.05f;
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        cameraPosition += glm::normalize(cameraLookAt) * -0.05f;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        cameraPosition += glm::normalize(cameraRight) * 0.05f;
    }
    else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        cameraPosition += glm::normalize(cameraRight) * -0.05f;
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        cameraPosition += glm::normalize(cameraUp) * 0.05f;
    }
    else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        cameraPosition += glm::normalize(cameraUp) * -0.05f;
    }
    else if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        if (!glm::epsilonEqual(cameraLookAt, cameraRotatedUp, 0.000001f).x) {
            cameraLookAt = glm::rotate(cameraLookAt, 0.05f, cameraRotatedUp);
        }
        if (!glm::epsilonEqual(cameraRight, cameraRotatedUp, 0.000001f).x) {
            cameraRight = glm::rotate(cameraRight, 0.05f, cameraRotatedUp);
        }
        if (!glm::epsilonEqual(cameraUp, cameraRotatedUp, 0.000001f).x) {
            cameraUp = glm::rotate(cameraUp, 0.05f, cameraRotatedUp);
        }
    }
    else if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        if (!glm::epsilonEqual(cameraLookAt, cameraRotatedUp, 0.000001f).x) {
            cameraLookAt = glm::rotate(cameraLookAt, -0.05f, cameraRotatedUp);
        }
        if (!glm::epsilonEqual(cameraRight, cameraRotatedUp, 0.000001f).x) {
            cameraRight = glm::rotate(cameraRight, -0.05f, cameraRotatedUp);
        }
        if (!glm::epsilonEqual(cameraUp, cameraRotatedUp, 0.000001f).x) {
            cameraUp = glm::rotate(cameraUp, -0.05f, cameraRotatedUp);
        }
    }
}

bool loadShader(GLuint* shader, GLenum type, const std::string& file) {
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
    *shader                  = glCreateShader(type);
    glShaderSource(*shader, 1, &shaderSource, nullptr);
    glCompileShader(*shader);

    return getShaderCompileStatus(*shader);
}

GLuint loadShaderProgram(const std::vector<std::pair<std::string, GLuint>>& shaders) {
    GLuint shaderProgram = glCreateProgram();
    std::vector<GLuint> handles;
    for (const auto& shader : shaders) {
        GLuint handle;
        if (!loadShader(&handle, shader.second, shader.first)) {
            return -1;
        }

        handles.push_back(handle);
        glAttachShader(shaderProgram, handle);
    }

    // Bind fragment shader output to the correct output buffer.
    glBindFragDataLocation(shaderProgram, 0, "outColour");

    // Link the shader program.
    glLinkProgram(shaderProgram);

    if (!getProgramLinkStatus(shaderProgram)) {
        for (const auto& shader : shaders) {
            std::cerr << shader.first << std::endl;
        }
        std::cerr << std::endl;
        return -1;
    }

    for (const auto& handle : handles) {
        glDetachShader(shaderProgram, handle);
        glDeleteShader(handle);
    }

    return shaderProgram;
}

bool getShaderCompileStatus(GLuint shader) {
    // Get status
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_TRUE) {
        return true;
    }

    else {
        // Get the length of the compile log.
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        // Get log.
        std::vector<char> buffer(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, buffer.data());
        std::cerr << "Vertex shader failed to compile." << std::endl;
        std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;

        return false;
    }
}

bool getProgramLinkStatus(GLuint program) {
    // Get status
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_TRUE) {
        return true;
    }

    else {
        // Get the length of the compile log.
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        // Get log.
        std::vector<char> buffer(logLength);
        glGetProgramInfoLog(program, logLength, &logLength, buffer.data());
        std::cerr << "Program failed to compile." << std::endl;
        std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;

        return false;
    }
}

void generateTerrain(float size, GLuint width, GLuint depth, float vertices[], GLuint elements[]) {
    // The vertices of a triangle.
    float triangle[16] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    for (GLuint i = 0; i < width; i++) {
        for (GLuint j = 0; j < depth; j++) {
            triangle[4]  = (i + 1) * size;
            triangle[10] = (j + 1) * size;
            triangle[12] = (i + 1) * size;
            triangle[14] = (j + 1) * size;

            std::memcpy(vertices + i * depth * 16 + j * 16, triangle, 16 * sizeof(float));
        }
    }

    for (GLuint i = 0; i < (width * depth); i++) {
        const GLuint base     = i * 4;
        elements[(i * 6) + 0] = base + 0;
        elements[(i * 6) + 1] = base + 1;
        elements[(i * 6) + 2] = base + 2;
        elements[(i * 6) + 3] = base + 1;
        elements[(i * 6) + 4] = base + 3;
        elements[(i * 6) + 5] = base + 2;
    }
}

glm::mat4x4 lookAt(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up) {
    const glm::vec3 f = glm::normalize(center - eye);
    const glm::vec3 s = glm::normalize(glm::cross(f, up));
    const glm::vec3 u = glm::cross(s, f);

    glm::mat4 Result(1);
    Result[0][0] = s.x;
    Result[1][0] = s.y;
    Result[2][0] = s.z;
    Result[0][1] = u.x;
    Result[1][1] = u.y;
    Result[2][1] = u.z;
    Result[0][2] = -f.x;
    Result[1][2] = -f.y;
    Result[2][2] = -f.z;
    Result[3][0] = -glm::dot(s, eye);
    Result[3][1] = -glm::dot(u, eye);
    Result[3][2] = glm::dot(f, eye);
    return Result;
}
