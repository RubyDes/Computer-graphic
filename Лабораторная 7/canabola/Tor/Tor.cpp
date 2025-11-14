#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float morphFactor;
uniform float time;

out vec3 fragColor;

// морфинг сферы в тор
vec3 morphPosition(float u, float v, float t) {

    float theta = u * 2.0 * 3.14159265;
    float phi = v * 3.14159265;
    
    vec3 spherePos = vec3(
        sin(phi) * cos(theta),
        cos(phi),
        sin(phi) * sin(theta)
    );
    
    float R = 1.5; // Большой радиус тора
    float r = 0.5; // Малый радиус тора
    float torusTheta = v * 2.0 * 3.14159265;
    
    vec3 torusPos = vec3(
        (R + r * cos(torusTheta)) * cos(theta),
        r * sin(torusTheta),
        (R + r * cos(torusTheta)) * sin(theta)
    );
    
    return mix(spherePos, torusPos, 0); // разобраться с тем что делает функция mix
}

void main() {
    float u = aPos.x;
    float v = aPos.y;
    
    float animatedMorph = (sin(time * 0.5) + 1.0) / 2.0;
    float finalMorph = mix(morphFactor, animatedMorph, 1.0);
    
    vec3 position = morphPosition(u, v, finalMorph);
    
    // Цвет
    vec3 sphereColor = vec3(1.0, 0.3, 0.2);
    vec3 torusColor = vec3(0.2, 0.5, 1.0);
    fragColor = mix(sphereColor, torusColor, finalMorph);
    
    gl_Position = projection * view * model * vec4(position, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec3 fragColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(fragColor, 1.0);
}
)";

// Глобальные переменные
GLFWwindow* window;
GLuint shaderProgram;
GLuint VAO, VBO, EBO;

float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;

bool mouseLeftPressed = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

float morphFactor = 0.0f;
bool autoAnimation = true;

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

void createParametricGrid(int uDivisions, int vDivisions) {
    std::vector<glm::vec2> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= uDivisions; ++i) {
        for (int j = 0; j <= vDivisions; ++j) {
            float u = float(i) / uDivisions;
            float v = float(j) / vDivisions;
            vertices.push_back(glm::vec2(u, v));
        }
    }

    for (int i = 0; i < uDivisions; ++i) {
        for (int j = 0; j < vDivisions; ++j) {
            int current = i * (vDivisions + 1) + j;
            int right = current + 1;
            int down = current + (vDivisions + 1);

            // Горизонтальные линии
            indices.push_back(current);
            indices.push_back(right);

            //// Вертикальные линии
            indices.push_back(current);
            indices.push_back(down);
        }
    }

    for (int i = 0; i < uDivisions; ++i) {
        int bottom = i * (vDivisions + 1);
        int top = bottom + vDivisions;
        indices.push_back(top);
        indices.push_back(bottom);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
        vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    std::cout << "Создано вершин: " << vertices.size() << std::endl;
    std::cout << "Создано индексов: " << indices.size() << std::endl;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mouseLeftPressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
        else {
            mouseLeftPressed = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouseLeftPressed) {
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;

        rotationY += deltaX * 0.5f;
        rotationX += deltaY * 0.5f;

        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

bool init() {
    if (!glfwInit()) {
        std::cout << "Ошибка инициализации GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1000, 800, "Tor", NULL, NULL);
    if (!window) {
        std::cout << "Ошибка создания окна GLFW" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (glewInit() != GLEW_OK) {
        std::cout << "Ошибка инициализации GLEW" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    return true;
}

// Основной цикл рендеринга
void render() {
    float lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processInput(window);

        //// Автоматическая анимация морфинга
        //if (autoAnimation) {
        //    morphFactor = (sin(currentTime * 0.5) + 1.0) / 2.0;
        //}


        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));


        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 4.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1000.0f / 800.0f, 0.1f, 100.0f);

        // Передача uniform переменных в шейдер
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(shaderProgram, "morphFactor"), morphFactor);
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), currentTime);

        // Отрисовка в режиме Wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(VAO);
        glDrawElements(GL_LINES, 100 * 100 * 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main() {
    if (!init()) {
        return -1;
    }

    createShaderProgram();
    createParametricGrid(50, 50);

    render();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}