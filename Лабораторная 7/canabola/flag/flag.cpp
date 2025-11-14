#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <cmath>

const int WIDTH = 800;
const int HEIGHT = 600;

// Вершинный шейдер
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 vUV;

void main()
{
    vUV = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

// Фрагментный шейдер
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec2 res;

in vec2 vUV;

void main()
{
    vec2 pixelCoord = vUV * res;
    
    vec2 center = res * 0.5;
    
    float rectWidth = 400.0;
    float rectHeight = 300.0;
    float borderWidth = 3.0;
    
    float left = center.x - rectWidth * 0.5;
    float right = center.x + rectWidth * 0.5;
    float top = center.y + rectHeight * 0.5;
    float bottom = center.y - rectHeight * 0.5;
    
    bool insideRect = (pixelCoord.x >= left && pixelCoord.x <= right) &&
                     (pixelCoord.y >= bottom && pixelCoord.y <= top);
    
    bool isBorder = insideRect && 
                   (pixelCoord.x <= left + borderWidth || 
                    pixelCoord.x >= right - borderWidth ||
                    pixelCoord.y <= bottom + borderWidth || 
                    pixelCoord.y >= top - borderWidth);
    
    if (isBorder) {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Синий цвет
        return;
    }

    float ringRadius = 60.0;
    float ringThickness = 6.0;

    //Second Ring
    float ringRadiusSecond = 30.0;
    float ringThicknessSecond = 6.0;
    
    vec2 toCenter = pixelCoord - center;
    float distance = length(toCenter);
    
    bool isRing = distance >= ringRadius && distance <= ringRadius + ringThickness;
    
    if (isRing) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Черный цвет
        return;
    }

    //SecondRing
    
    bool isRingSecond = distance >= ringRadiusSecond && distance <= ringRadiusSecond + ringThicknessSecond;
    
    if (isRingSecond) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Черный цвет
        return;
    }
    
    FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

GLuint shaderProgram;
GLuint VAO, VBO;


GLuint compileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

void createShaders() {
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void setupGeometry() {
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Blue Rectangle with Black Ring", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, NULL);

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    createShaders();
    setupGeometry();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);


        GLuint resLoc = glGetUniformLocation(shaderProgram, "res");
        glUniform2f(resLoc, (float)WIDTH, (float)HEIGHT);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}