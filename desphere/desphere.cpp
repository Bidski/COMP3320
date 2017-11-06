#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>  // Include this first
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int FORMAT_RGB3 = 0x33424752;

// bayer formats
const int FORMAT_GRBG = 0x47425247;
const int FORMAT_RGGB = 0x42474752;
const int FORMAT_GBRG = 0x47524247;
const int FORMAT_BGGR = 0x52474742;

GLFWwindow* init(bool windowed, size_t width, size_t height);
static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLuint loadShaderProgram(const std::vector<std::pair<std::string, GLuint>>& shaders);
bool loadShader(GLuint* shader, GLenum type, const std::string& file);
bool getShaderCompileStatus(GLuint shader);
bool getProgramLinkStatus(GLuint program);
void createGeometry(int width,
                    int height,
                    std::vector<GLuint>& indices,
                    std::vector<float>& vertices,
                    std::vector<float>& normals,
                    std::vector<float>& uvs);
void loadImage(const std::string& file,
               std::array<uint32_t, 2>& dimensions,
               std::vector<uint8_t>& image,
               bool& RGB,
               uint8_t& bytes_per_pixel);
void saveImage(const std::string& file,
               std::array<uint32_t, 2> dimensions,
               const std::vector<uint8_t>& image,
               bool RGB,
               uint8_t bytes_per_pixel);
void dumpError(int line);

int main(int argc, char** argv) {
    // Set error callback.
    glfwSetErrorCallback(error_callback);

    GLFWwindow* window = init(true, 800, 600);

    if (window == nullptr) {
        return (-1);
    }

    // Set keypress callback.
    glfwSetKeyCallback(window, key_callback);

    // Enable depth testing.
    glEnable(GL_DEPTH_TEST);

    // Create Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object (VBO).
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Specify the vertices.
    // clang-format off
    float vertices[] = {
        -400,  300, 0, 0, 0, 1, 0, 1,
         400,  300, 0, 0, 0, 1, 1, 1,
        -400, -300, 0, 0, 0, 1, 0, 0,
         400, -300, 0, 0, 0, 1, 1, 0
    };
    GLuint indices[] = {
        0, 2, 1,
        2, 3, 1
    };
    // clang-format on

    // Copy vertex data into the VBO.
    // GL_STATIC_DRAW: Copy vertex data to graphics card once, then redraw many times.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an Element Buffer Object (EBO).
    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    // Create the shader program.
    GLuint shaderProgram = loadShaderProgram({{"desphere.vs", GL_VERTEX_SHADER}, {"desphere.fs", GL_FRAGMENT_SHADER}});

    if (shaderProgram < 0) {

        std::cerr << "Failed to link shader program!" << std::endl;
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glfwTerminate();
        return -1;
    }

    // Start using the shader program.
    glUseProgram(shaderProgram);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

    GLint normAttrib = glGetAttribLocation(shaderProgram, "normal");
    if (normAttrib > -1) {
        glEnableVertexAttribArray(normAttrib);
        glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
    }

    GLint uvAttrib = glGetAttribLocation(shaderProgram, "uv");
    glEnableVertexAttribArray(uvAttrib);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (6 * sizeof(GLfloat)));

    // Create model, view and projection matrices.
    GLint uniProj  = glGetUniformLocation(shaderProgram, "proj");
    GLint uniView  = glGetUniformLocation(shaderProgram, "view");
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, 0.1f, 10.0f);
    glm::mat4 model = glm::mat4(1.0);

    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

    std::string imageFile(argv[1]);
    std::array<uint32_t, 2> dimensions;
    std::vector<uint8_t> image;
    bool RGB;
    uint8_t bytes_per_pixel;

    loadImage(imageFile, dimensions, image, RGB, bytes_per_pixel);

    GLint uniImageFormat          = glGetUniformLocation(shaderProgram, "imageFormat");
    GLint uniImageWidth           = glGetUniformLocation(shaderProgram, "imageWidth");
    GLint uniImageHeight          = glGetUniformLocation(shaderProgram, "imageHeight");
    GLint uniResolution           = glGetUniformLocation(shaderProgram, "resolution");
    GLint uniFirstRed             = glGetUniformLocation(shaderProgram, "firstRed");
    GLint uniRadiansPerPixel      = glGetUniformLocation(shaderProgram, "radiansPerPixel");
    GLint uniCamFocalLengthPixels = glGetUniformLocation(shaderProgram, "camFocalLengthPixels");

    GLfloat dims[2] = {GLfloat(dimensions[0]), GLfloat(dimensions[1])};
    GLfloat red[2]  = {0.0f, 0.0f};
    glUniform1i(uniImageFormat, (RGB ? FORMAT_RGB3 : FORMAT_RGGB));
    glUniform1i(uniImageWidth, 800);
    glUniform1i(uniImageHeight, 600);
    glUniform2fv(uniResolution, 1, dims);
    glUniform2fv(uniFirstRed, 1, red);
    glUniform1f(uniRadiansPerPixel, 0.0026997136600899543f);

    float camFocalLengthPixels = std::sqrt((800.0f * 800.0f) + (600.0f + 600.0f)) / std::tan(150.0f * M_PI / 360.0f);
    glUniform1f(uniCamFocalLengthPixels, camFocalLengthPixels);

    // Load the texture.
    GLuint tex;
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 dimensions[0],
                 dimensions[1],
                 0,
                 (RGB ? GL_RGB : GL_RED),
                 GL_UNSIGNED_BYTE,
                 image.data());
    glUniform1i(glGetUniformLocation(shaderProgram, "rawImage"), 0);

    // Set texture parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Main event loop.
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // Check for the escape key.
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Clear the screen to black.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for window events.
        glfwPollEvents();
    }


    // Cleanup.
    glDeleteTextures(1, &tex);
    glDeleteProgram(shaderProgram);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

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

GLuint loadShaderProgram(const std::vector<std::pair<std::string, GLuint>>& shaders) {
    GLuint shaderProgram = glCreateProgram();
    std::vector<GLuint> handles;
    for (const auto& shader : shaders) {
        std::cerr << "Loading shader '" << shader.first << "' ..." << std::endl;
        GLuint handle;
        if (!loadShader(&handle, shader.second, shader.first)) {
            std::cerr << "Shader failed to compile?" << std::endl;
            return -1;
        }

        std::cerr << "Adding shader to program." << std::endl;
        handles.push_back(handle);
        glAttachShader(shaderProgram, handle);
    }

    std::cerr << "All shaders loaded successfully." << std::endl;

    // Bind fragment shader output to the correct output buffer.
    glBindFragDataLocation(shaderProgram, 0, "outColour");

    // Link the shader program.
    glLinkProgram(shaderProgram);

    if (!getProgramLinkStatus(shaderProgram)) {
        std::cerr << "Failed to link shader program." << std::endl;
        for (const auto& shader : shaders) {
            std::cerr << shader.first << std::endl;
        }
        std::cerr << std::endl;
        return -1;
    }

    std::cerr << "Shader program linked successfully." << std::endl;

    for (const auto& handle : handles) {
        glDetachShader(shaderProgram, handle);
        glDeleteShader(handle);
    }

    return shaderProgram;
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
        // Get the length of the compile log.
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        // Get log.
        std::vector<char> buffer(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, buffer.data());
        std::cerr << "Shader compiled successfully." << std::endl;
        if (buffer.size() > 0) {
            std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;
        }

        return true;
    }

    else {
        // Get the length of the compile log.
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        // Get log.
        std::vector<char> buffer(logLength);
        glGetShaderInfoLog(shader, logLength, nullptr, buffer.data());
        std::cerr << "Shader failed to compile." << std::endl;
        if (buffer.size() > 0) {
            std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;
        }

        return false;
    }
}

bool getProgramLinkStatus(GLuint program) {
    // Get status
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_TRUE) {
        // Get the length of the compile log.
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        // Get log.
        std::vector<char> buffer(logLength);
        glGetProgramInfoLog(program, logLength, &logLength, buffer.data());
        std::cerr << "Program compiled successfully." << std::endl;
        if (buffer.size() > 0) {
            std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;
        }

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
        if (buffer.size() > 0) {
            std::cerr << "Compile Log:" << std::endl << buffer.data() << std::endl;
        }

        return false;
    }
}

void loadImage(const std::string& file,
               std::array<uint32_t, 2>& dimensions,
               std::vector<uint8_t>& image,
               bool& RGB,
               uint8_t& bytes_per_pixel) {
    std::ifstream ifs(file, std::ios::in | std::ios::binary);
    std::string magic_number, width, height, max_val;
    ifs >> magic_number;

    if (magic_number.compare("P6") == 0) {
        RGB = true;
    }

    else if (magic_number.compare("P5") == 0) {
        RGB = false;
    }

    else {
        throw std::runtime_error("Image has incorrect format.");
        return;
    }

    ifs >> width >> height >> max_val;
    dimensions[0]   = std::stoi(width);
    dimensions[1]   = std::stoi(height);
    bytes_per_pixel = ((std::stoi(max_val) > 255) ? 2 : 1) * (RGB ? 3 : 1);

    image.resize(dimensions[0] * dimensions[1] * bytes_per_pixel, 0);

    // Skip data in file until a whitespace character is found.
    while (ifs && !std::isspace(ifs.peek()))
        ifs.ignore();
    // Skip all whitespace until we find non-whtespace.
    while (ifs && std::isspace(ifs.peek()))
        ifs.ignore();

    ifs.read(reinterpret_cast<char*>(image.data()), image.size());

    ifs.close();
}

void saveImage(const std::string& file,
               std::array<uint32_t, 2> dimensions,
               const std::vector<uint8_t>& image,
               bool RGB,
               uint8_t bytes_per_pixel) {
    std::ofstream ofs(file, std::ios::out | std::ios::binary);

    if ((dimensions[0] * dimensions[1] * bytes_per_pixel) != image.size()) {
        throw std::runtime_error("Image has incorrect dimensions.");
        return;
    }

    ofs << "P" << (RGB ? "6" : "5") << "\n";
    ofs << std::to_string(dimensions[0]) << " " << std::to_string(dimensions[1]) << "\n";
    ofs << "255\n";
    ofs.write(reinterpret_cast<const char*>(image.data()), image.size());

    ofs.close();
}

void dumpError(int line) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        switch (err) {
            case GL_INVALID_ENUM:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "GL_INVALID_ENUM" << std::endl;
                std::cerr << "Given when an enumeration parameter is not a legal enumeration for that function. "
                          << "This is given only for local problems; if the spec allows the enumeration in certain "
                          << "circumstances, where other parameters or state dictate those circumstances, then "
                          << "GL_INVALID_OPERATION is the result instead." << std::endl;
                break;
            case GL_INVALID_VALUE:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "GL_INVALID_VALUE" << std::endl;
                std::cerr << "Given when a value parameter is not a legal value for that function. "
                          << "This is only given for local problems; if the spec allows the value in certain "
                          << "circumstances, where other parameters or state dictate those circumstances, "
                          << "then GL_INVALID_OPERATION is the result instead." << std::endl;
                break;
            case GL_INVALID_OPERATION:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "GL_INVALID_OPERATION" << std::endl;
                std::cerr << "Given when the set of state for a command is not legal for the parameters given to that "
                          << "command. It is also given for commands where combinations of parameters define what the "
                          << "legal parameters are." << std::endl;
                break;
            case GL_STACK_OVERFLOW:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "GL_STACK_OVERFLOW" << std::endl;
                std::cerr << "Given when a stack pushing operation cannot be done because it would overflow the "
                          << "limit of that stack's size." << std::endl;
                break;
            case GL_STACK_UNDERFLOW:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "GL_STACK_UNDERFLOW" << std::endl;
                std::cerr << "Given when a stack popping operation cannot be done because the stack is already at "
                          << "its lowest point." << std::endl;
                break;
            case GL_OUT_OF_MEMORY:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
                std::cerr << "Given when performing an operation that can allocate memory, and the memory cannot be "
                          << "allocated. The results of OpenGL functions that return this error are undefined; "
                          << "it is allowable for partial operations to happen." << std::endl;
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
                std::cerr << "Given when doing anything that would attempt to read from or write/render to a "
                          << "framebuffer that is not complete." << std::endl;
                break;
            case GL_CONTEXT_LOST:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "GL_CONTEXT_LOST" << std::endl;
                std::cerr << "Given if the OpenGL context has been lost, due to a graphics card reset." << std::endl;
                break;
            default:
                std::cerr << __FILE__ << ":" << line << std::endl;
                std::cerr << "Unknown error: " << err << std::endl;
                break;
        }
    }
}
