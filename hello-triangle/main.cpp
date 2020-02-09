#include <iostream>
#include <cmath>

// GLEW and GLFW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLint WIDTH = 800, HEIGHT = 600;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 vertexColor;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   vertexColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 vertexColor;\n"
    "void main() {\n"
    "   FragColor = vec4(vertexColor, 1.0f);"
    "}\0";

// Check for compile and linking errors
void checkVertexShaderCompileError(unsigned int &vertexShader, char infoLog[], int &success);
void checkFragmentShaderCompileError(unsigned int &fragmentShader, char infoLog[], int &success);
void checkLinkingError(unsigned int &shaderProgram, char infoLog[], int &success);

int main() {
    glfwInit();
    
    // Set OpenGL window to version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    
    // Use OpenGL core and enable macOS compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Test", nullptr, nullptr);
    
    // Account for macOS retina resolution
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    
    if(GLEW_OK != glewInit()) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    
    glViewport(0, 0, screenWidth, screenHeight);
    
    int success;
    char infoLog[512];
    
    // Build and compile shaders
    // Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    unsigned int shaderProgram = glCreateProgram();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // Check for shader compile errors
    checkVertexShaderCompileError(vertexShader, infoLog, success);
    checkFragmentShaderCompileError(fragmentShader, infoLog, success);
    
    // Link shaders
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    checkLinkingError(shaderProgram, infoLog, success);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    //  Set up vertex and buffer data, and configure vertex attributes
    float triangle1[] = {
        // Positions        // Colors
        0.5f,  -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
    };
    float triangle2[] = {
        // Positions        // Colors
        0.0f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        0.9f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.45f, 0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
    };
    
    unsigned int VBOs[2], VAOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    
    // First triangle
    // Bind Vertex Array Object, then bind and set vertex buffer(s)
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW);
    
    // Configure vertex position attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Configure vertex color attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Second triangle
    // Bind Vertex Array Object, then bind and set vertex buffer(s)
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);
    
    // Configure vertex position attributes
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Configure vertex color attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw triangles
        glUseProgram(shaderProgram);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
//        glBindVertexArray(VAOs[1]);
//        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // Use double buffer
        // Only swap old frame with new when it is completed
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, VAOs);
    glDeleteBuffers(1, VBOs);
    
    glfwTerminate();
    return 0;
}

void checkLinkingError(unsigned int &shaderProgram, char infoLog[], int &success) {
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
          glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
          std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

void checkVertexShaderCompileError(unsigned int &vertexShader, char infoLog[], int &success) {
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void checkFragmentShaderCompileError(unsigned int &fragmentShader, char infoLog[], int &success) {
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}
