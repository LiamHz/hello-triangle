#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"

const GLint WIDTH = 800, HEIGHT = 600;

// Functions
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// Camera
Camera camera(glm::vec3(0.0f, 0.0f,  3.0f));
bool firstMouse = true;
float lastX = WIDTH / 2;
float lastY = HEIGHT / 2;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame;

int main() {
    glfwInit();

    // Set OpenGL window to version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Use OpenGL core and enable macOS compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Learn OpenGL", nullptr, nullptr);

    // Account for macOS retina resolution
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, screenWidth, screenHeight);

    // Build and compile shaders
    Shader shader("shader.vs", "shader.fs");
    Shader lampShader("shader.vs", "lamp.fs");
    

    float vertices[] = {
        // Postion            // Surface normals
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  -2.0f),
        glm::vec3( 2.0f,  5.0f, -5.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -6.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-2.3f,  0.0f, -2.5f),
        glm::vec3(-0.3f,  2.0f, -3.5f),
        glm::vec3(3.3f,  1.0f, -1.5f)
    };

    unsigned int VBO, VAO, lightCubeVAO;
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Configure vertex position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Configure vertex texture attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Configure vertex position attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glm::vec3 lightPos(1.2f, 4.0f, 4.0f);
    
    // Set light intensities
    shader.use();
    shader.setVec3("light.ambient",  0.5f, 0.5f, 0.5f);
    shader.setVec3("light.diffuse",  0.5f, 0.5f, 0.5f);
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // Set projection matrix
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    // Enable wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Enable z-buffer
    glEnable(GL_DEPTH_TEST);
    
    // Enable mouse input
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    std::vector<glm::vec3> ambient;
    std::vector<glm::vec3> diffuse;
    std::vector<glm::vec3> specular;
    std::vector<float> shininess;
    
    // Emerald
    ambient.push_back(glm::vec3{0.0215, 0.1745, 0.0215});
    diffuse.push_back(glm::vec3{0.07568, 0.61424, 0.07568});
    specular.push_back(glm::vec3{0.633, 0.727811, 0.633});
    shininess.push_back(0.6);
    
    // Jade
    ambient.push_back(glm::vec3{0.135, 0.2225, 0.1575});
    diffuse.push_back(glm::vec3{0.54, 0.89, 0.63});
    specular.push_back(glm::vec3{0.316228, 0.316228, 0.316228});
    shininess.push_back(0.1);
    
    // Obsidian
    ambient.push_back(glm::vec3{0.05375, 0.05, 0.06625});
    diffuse.push_back(glm::vec3{0.18275, 0.17, 0.22525});
    specular.push_back(glm::vec3{0.332741, 0.328634, 0.346435});
    shininess.push_back(0.3);
    
    // Pearl
    ambient.push_back(glm::vec3{0.25, 0.20725, 0.20725});
    diffuse.push_back(glm::vec3{1, 0.829, 0.829});
    specular.push_back(glm::vec3{0.296648, 0.296648, 0.296648});
    shininess.push_back(0.088);
    
    // Ruby
    ambient.push_back(glm::vec3{0.1745, 0.01175, 0.01175});
    diffuse.push_back(glm::vec3{0.61424, 0.04136, 0.04136});
    specular.push_back(glm::vec3{0.727811, 0.626959, 0.626959});
    shininess.push_back(0.6);
    
    // Turqouise
    ambient.push_back(glm::vec3{0.1, 0.18725, 0.1745});
    diffuse.push_back(glm::vec3{0.396, 0.74151, 0.69102});
    specular.push_back(glm::vec3{0.297254, 0.30829, 0.306678});
    shininess.push_back(0.1);
    
    // Brass
    ambient.push_back(glm::vec3{0.329412, 0.223529, 0.027451});
    diffuse.push_back(glm::vec3{0.780392, 0.568627, 0.113725});
    specular.push_back(glm::vec3{0.992157, 0.941176, 0.807843});
    shininess.push_back(0.21794872);
    
    // Bronze
    ambient.push_back(glm::vec3{0.2125, 0.1275, 0.054});
    diffuse.push_back(glm::vec3{0.714, 0.4284, 0.18144});
    specular.push_back(glm::vec3{0.393548, 0.271906, 0.166721});
    shininess.push_back(0.2);
    
    // Chrome
    ambient.push_back(glm::vec3{0.25, 0.25, 0.25});
    diffuse.push_back(glm::vec3{0.4, 0.4, 0.4});
    specular.push_back(glm::vec3{0.774597, 0.774597, 0.774597});
    shininess.push_back(0.6);
    
    // Copper
    ambient.push_back(glm::vec3{0.19125, 0.0735, 0.0225});
    diffuse.push_back(glm::vec3{0.7038, 0.27048, 0.0828});
    specular.push_back(glm::vec3{0.256777, 0.137622, 0.086014});
    shininess.push_back(0.1);
    
    // Gold
    ambient.push_back(glm::vec3{0.24725, 0.1995, 0.0745});
    diffuse.push_back(glm::vec3{0.75164, 0.60648, 0.22648});
    specular.push_back(glm::vec3{0.628281, 0.555802, 0.366065});
    shininess.push_back(0.4);
    
    // Silver
    ambient.push_back(glm::vec3{0.19225, 0.19225, 0.19225});
    diffuse.push_back(glm::vec3{0.50754, 0.50754, 0.50754});
    specular.push_back(glm::vec3{0.508273, 0.508273, 0.508273});
    shininess.push_back(0.4);
    
    
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader
        shader.use();

        // Pass projection matrix to shader
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // Camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        shader.setVec3("viewPos", camera.Position);
        
        // Render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < ambient.size(); i++) {
            // Set world coordinates of object
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 15.0f * (i+1);
            if (i == 0) { angle = 40.0f; }
                model = glm::rotate(model, (float)glfwGetTime() * glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);
            shader.setVec3("material.ambient", ambient[i]);
            shader.setVec3("material.diffuse", diffuse[i]);
            shader.setVec3("material.specular", specular[i]);
            shader.setFloat("material.shininess", shininess[i]);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        lampShader.use();
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lampShader.setMat4("model", model);
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Use double buffer
        // Only swap old frame with new when it is completed
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    // Prevent camera jumping when mouse first enters screen
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    // yoffset is reversed since y-coords go from bottom to top
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
