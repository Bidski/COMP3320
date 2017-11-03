#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>  // Include this first
#include <GLFW/glfw3.h>

GLFWwindow* init(bool windowed, size_t width, size_t height);
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool loadShader(GLuint* shader, GLenum type, const std::string& file);
bool getShaderCompileStatus(GLuint shader);
void createGeometry(int width,
                    int height,
                    std::vector<GLuint>& indices,
                    std::vector<float>& vertices,
                    std::vector<float>& normals,
                    std::vector<float>& uvs);

int main(void) {
    // Set error callback.
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = init(true, 800, 600);

    if (window == nullptr) {
        return (-1);
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
    std::vector<float> vertices, normals, uvs;
    std::vector<GLuint> indices;
    createGeometry(800, 600, indices, vertices, normals, uvs);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Copy vertex data into the VBO.
    // GL_STATIC_DRAW: Copy vertex data to graphics card once, then redraw many times.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices::value_type), vertices.data(), GL_STATIC_DRAW);

    // Create an Element Buffer Object (EBO).
    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices::value_type), indices.data(), GL_STATIC_DRAW);

    // Create the vertex shader.
    GLuint vertexShader;

    if (loadShader(&vertexShader, GL_VERTEX_SHADER, "desphere.vs") == false) {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        glfwTerminate();

        return (-1);
    }

    // Create the fragment shader.
    GLuint fragmentShader;

    if (loadShader(&fragmentShader, GL_FRAGMENT_SHADER, "desphere.fs") == false) {
        glDeleteShader(vertexShader);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);

        glfwTerminate();

        return (-1);
    }

    // Create the shader program.
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Bind fragment shader output to the correct output buffer.
    glBindFragDataLocation(shaderProgram, 0, "gl_FragColor");

    // Link the shader program.
    glLinkProgram(shaderProgram);

    // Start using the shader program.
    glUseProgram(shaderProgram);

    // Create model, view and projection matrices.
    GLint uniProj  = glGetUniformLocation(shaderProgram, "proj");
    GLint uniView  = glGetUniformLocation(shaderProgram, "view");
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    glm::mat4 uniView  = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 uniProj  = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, 1.0f, 1000.0f);
    glm::mat4 uniModel = glm::mat4(1.0);

    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(model));

    // Main event loop.
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // Check for the escape key.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Clear the screen to black.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for window events.
        glfwPollEvents();
    }

    // Cleanup.
    glDeleteBuffers(1, &vertexBuffer);

    glfwTerminate();

    return (0);
}

GLFWwindow* init(bool windowed, size_t width, size_t height) {
    // Initialise GLFW.
    if (glfwInit() == GL_FALSE) {
        std::cerr << "Failed to initialize GLFW" << std::endl;

        return (nullptr);
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

    if (windowed == true) {
        window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);
    }

    else {
        window = glfwCreateWindow(width, height, "OpenGL", glfwGetPrimaryMonitor(), nullptr);
    }

    if (window == nullptr) {
        std::cerr << "Failed to create window." << std::endl;

        glfwTerminate();
        return (nullptr);
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

    return (window);
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

    return (getShaderCompileStatus(*shader));
}

bool getShaderCompileStatus(GLuint shader) {
    // Get status
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_TRUE) {
        return (true);
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

        return (false);
    }
}

void createGeometry(int width,
                    int height,
                    std::vector<GLuint>& indices,
                    std::vector<float>& vertices,
                    std::vector<float>& normals,
                    std::vector<float>& uvs) {
    float width_half  = width * 0.5f;
    float height_half = height * 0.5f;

    int gridX  = 1;
    int gridY  = 1;
    int gridX1 = gridX + 1;
    int gridY1 = gridY + 1;

    float segment_width  = width / float(gridX);
    float segment_height = height / float(gridY);

    // buffers
    indices.clear();
    vertices.clear();
    normals.clear();
    uvs.clear();

    // generate vertices, normals and uvs
    for (int iy = 0; iy < gridY1; iy++) {
        float y = iy * segment_height - height_half;

        for (int ix = 0; ix < gridX1; ix++) {
            float x = ix * segment_width - width_half;

            vertices.push_back(x);
            vertices.push_back(-y);
            vertices.push_back(0.0f);

            normals.push_back(0.0f);
            normals.push_back(0.0f);
            normals.push_back(1.0f);

            uvs.push_back(ix / gridX);
            uvs.push_back(1 - (iy / gridY));
        }
    }

    // indices
    for (int iy = 0; iy < gridY; iy++) {
        for (int ix = 0; ix < gridX; ix++) {
            GLuint a = ix + gridX1 * iy;
            GLuint b = ix + gridX1 * (iy + 1);
            GLuint c = (ix + 1) + gridX1 * (iy + 1);
            GLuint d = (ix + 1) + gridX1 * iy;

            // faces
            indices.push(a);
            indices.push(b);
            indices.push(d);
            indices.push(b);
            indices.push(c);
            indices.push(d);
        }
    }

    // build geometry
    this.setIndex(indices);
    this.addAttribute('position', new Float32BufferAttribute(vertices, 3));
    this.addAttribute('normal', new Float32BufferAttribute(normals, 3));
    this.addAttribute('uv', new Float32BufferAttribute(uvs, 2));
}
