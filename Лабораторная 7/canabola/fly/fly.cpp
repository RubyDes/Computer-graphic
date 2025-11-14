#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
out vec2 fragCoord;

void main() {
    fragCoord = aPos;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 fragCoord;
out vec4 FragColor;

uniform vec2 center;
uniform float zoom;

void main() {
    vec2 uv = (fragCoord - 0.5) * 2.0;
    
    vec2 c = uv * zoom + center;
    
    vec2 z = vec2(0.0, 0.0);
    int iterations = 0;
    const int max_iterations = 150;
    
    for (int i = 0; i < max_iterations; i++) {
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c; //сделать так чтобы цвет выбирался от количества итераций. и сравнивать не с числом 4, а , например 1000000. Сделать так чтобы zoom происходил в центр картинки
        
        if (dot(z, z) > 4.0) {
            break;
        }
        iterations++;
    }

    if (iterations == max_iterations) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        FragColor = vec4(0.1, 0.3, 0.8, 1.0);
    }
}
)";

GLFWwindow* window;
GLuint shaderProgram;
GLuint VAO, VBO;

float cameraX = 0.0f;
float cameraY = 0.0f;
float zoom = 1.0f;

float lastFrameTime = 0.0f;
bool keys[512] = { false };

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Ошибка компиляции шейдера:\n" << infoLog << std::endl;
    }
    return shader;
}

void createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Ошибка линковки шейдерной программы:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void createFullscreenQuad() {
    float vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
    }
    else if (action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

void updateCamera(float deltaTime) {
    float moveSpeed = 1.0f * zoom * deltaTime;
    float zoomSpeed = 1.0f + 2.0f * deltaTime;

    if (keys[GLFW_KEY_LEFT])
        cameraX -= moveSpeed;
    if (keys[GLFW_KEY_RIGHT])
        cameraX += moveSpeed;
    if (keys[GLFW_KEY_UP])
        cameraY += moveSpeed;
    if (keys[GLFW_KEY_DOWN])
        cameraY -= moveSpeed;

    if (keys[GLFW_KEY_U])
        zoom /= zoomSpeed;
    if (keys[GLFW_KEY_D])
        zoom *= zoomSpeed;

    if (keys[GLFW_KEY_R]) {
        cameraX = 0.0f;
        cameraY = 0.0f;
        zoom = 1.0f;
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

bool init() {
    if (!glfwInit()) {
        std::cout << "Ошибка инициализации GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1200, 800, "Fractal", NULL, NULL);
    if (!window) {
        std::cout << "Ошибка создания окна GLFW" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    if (glewInit() != GLEW_OK) {
        std::cout << "Ошибка инициализации GLEW" << std::endl;
        return false;
    }
    return true;
}

void render() {
    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastFrameTime;
        lastFrameTime = currentTime;

        processInput(window);
        updateCamera(deltaTime);

        glUseProgram(shaderProgram);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        // Передаем uniform переменные
        glUniform2f(glGetUniformLocation(shaderProgram, "center"), cameraX, cameraY);
        glUniform1f(glGetUniformLocation(shaderProgram, "zoom"), zoom);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main() {
    if (!init()) {
        return -1;
    }

    createShaderProgram();
    createFullscreenQuad();
    render();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}