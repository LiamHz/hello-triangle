#include <iostream>
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include <filesystem>

const GLint WIDTH = 800, HEIGHT = 600;

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
    
    // Build and compile shaders
    Shader shader("shader.vs", "shader.fs");
    // Set up vertex and buffer data, and configure vertex attributes
    float triangle1[] = {
        // Positions        // Colors
        0.5f,  -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top
    };
    
    unsigned int VBOs[1], VAOs[1];
    glGenVertexArrays(1, VAOs);
    glGenBuffers(1, VBOs);
    
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

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw triangles
        shader.use();
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
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
