#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cmath>

#include <GL/glew.h>                // Include this first
#include <GLFW/glfw3.h>

#include <SOIL/SOIL.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    // Create Vertex Array Object (VAO).
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create a Vertex Buffer Object (VBO).
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // Specify the vertices.
    float vertices[] = 
    {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0, 0.0, // Top-left
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0, 0.0, // Top-right
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0, 1.0, // Bottom-right
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0, 1.0  // Bottom-left
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Copy vertex data into the VBO.
    // GL_STATIC_DRAW: Copy vertex data to graphics card once, then redraw many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create an Element Buffer Object (EBO).
    GLuint ebo;
    glGenBuffers(1, &ebo);
    
    GLuint elements[] = 
    {
        0, 1, 2,
        2, 3, 0
    };
   
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

    // Create the vertex shader.
    std::ifstream vertexSourceFile("vertexShader.glsl");
    std::string vertexSource;

    vertexSourceFile.seekg(0, std::ios::end);   
    vertexSource.reserve(vertexSourceFile.tellg());
    vertexSourceFile.seekg(0, std::ios::beg);

    vertexSource.assign((std::istreambuf_iterator<char>(vertexSourceFile)), std::istreambuf_iterator<char>());

    vertexSourceFile.close();
    const char* vertexShaderSource = vertexSource.c_str();

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
    std::string fragmentSource;

    fragmentSourceFile.seekg(0, std::ios::end);   
    fragmentSource.reserve(fragmentSourceFile.tellg());
    fragmentSourceFile.seekg(0, std::ios::beg);

    fragmentSource.assign((std::istreambuf_iterator<char>(fragmentSourceFile)), std::istreambuf_iterator<char>());

    fragmentSourceFile.close();
    const char* fragmentShaderSource = fragmentSource.c_str();

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
        std::cerr << "Fragment shader failed to compile." << std::endl;
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
    // There are 7 elements for each vector, the first two are the (X, Y) coordinates.
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

    // Tell OpenGL where to find the colour attributes in the vertex data.
    // There are 7 elements for each vector, the 3rd, 4th, and 5th are the (R, G, B) values.
    GLint colAttrib = glGetAttribLocation(shaderProgram, "colour");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

    // Tell OpenGL where to find the texture attributes in the vertex data.
    // There are 7 elements for each vector, the last two are the (S, T) values.
    GLint texAttrib = glGetAttribLocation(shaderProgram, "textureCoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

    // Load the texture.
    GLuint textures[2];
    glGenTextures(2, textures);

    // Load the image for the texture.
    int width, height;
    unsigned char* image = NULL;
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    image = SOIL_load_image("../sample.png", &width, &height, 0, SOIL_LOAD_RGB);

    if (image == NULL)
    {
        std::cerr << "Failed to load texture file." << std::endl;

        glDeleteTextures(2, textures);
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glfwTerminate();

        return(-1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    image = NULL;
    glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    image = SOIL_load_image("../sample2.png", &width, &height, 0, SOIL_LOAD_RGB);

    if (image == NULL)
    {
        std::cerr << "Failed to load texture file." << std::endl;

        glDeleteTextures(2, textures);
        glDeleteProgram(shaderProgram);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
        glfwTerminate();

        return(-1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);
    glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Create the transform.
    glm::mat4 trans;
    trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec4 result = trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    std::cout << "(" << result.x << ", " << result.y << ", " << result.z << ")" << std::endl;

    // Get access to the uniform in the vertex shader.
    GLint uniTrans = glGetUniformLocation(shaderProgram, "trans");

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

        // Perform the tranformation.
        glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(trans));

        // Draw a rectangle from the 2 triangles using 6 indices.
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Swap front and back buffers.
        glfwSwapBuffers(window);

        // Poll for window events.
        glfwPollEvents();
    }

    // Cleanup.
    glDeleteTextures(2, textures);
    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwTerminate();

    return(0);
}

