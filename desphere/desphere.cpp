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
bool loadShader(GLuint* shader, GLenum type, const std::string& file);
bool getShaderCompileStatus(GLuint shader);
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

int main(int argc, char** argv) {
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
    GLuint vbo[3];
    glGenBuffers(3, vbo);

    // Specify the vertices.
    std::vector<float> vertices, normals, uvs;
    std::vector<GLuint> indices;
    createGeometry(800, 600, indices, vertices, normals, uvs);


    // Copy vertex data into the VBO.
    // GL_STATIC_DRAW: Copy vertex data to graphics card once, then redraw many times.
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);

    // Create an Element Buffer Object (EBO).
    GLuint ebo;
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Create the vertex shader.
    GLuint vertexShader;

    if (loadShader(&vertexShader, GL_VERTEX_SHADER, "desphere.vs") == false) {
        glDeleteBuffers(3, vbo);
        glDeleteVertexArrays(1, &vao);

        glfwTerminate();

        return (-1);
    }

    // Create the fragment shader.
    GLuint fragmentShader;

    if (loadShader(&fragmentShader, GL_FRAGMENT_SHADER, "desphere.fs") == false) {
        glDeleteShader(vertexShader);
        glDeleteBuffers(3, vbo);
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

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint normAttrib = glGetAttribLocation(shaderProgram, "normal");
    glEnableVertexAttribArray(normAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "uv");
    glEnableVertexAttribArray(texAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Create model, view and projection matrices.
    GLint uniProj  = glGetUniformLocation(shaderProgram, "proj");
    GLint uniView  = glGetUniformLocation(shaderProgram, "view");
    GLint uniModel = glGetUniformLocation(shaderProgram, "model");

    glm::mat4 view  = glm::lookAt(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 proj  = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, 1.0f, 1000.0f);
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
    glUniform2fv(uniResolution, 2, dims);
    glUniform2fv(uniFirstRed, 2, red);
    glUniform1f(uniRadiansPerPixel, 0.0026997136600899543f);

    float camFocalLengthPixels = std::sqrt((800 * 800) + (600 + 600)) / std::tan(150.0f * M_PI / 360.0f);
    glUniform1f(uniCamFocalLengthPixels, camFocalLengthPixels);

    // Load the texture.
    GLuint tex;
    glGenTextures(1, &tex);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 dimensions[0],
                 dimensions[1],
                 0,
                 (RGB ? GL_RGB : GL_RED),
                 GL_UNSIGNED_BYTE,
                 image.data());

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
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for window events.
        glfwPollEvents();
    }

    // Cleanup.
    glDeleteTextures(1, &tex);
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(3, vbo);
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
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(d);
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
        }
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
