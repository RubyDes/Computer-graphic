#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Шейдерные исходники (встроены в код для простоты)
const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
out vec4 FragColor;
uniform vec3 starColor;
void main() {
    FragColor = vec4(starColor, 1.0);
}
)glsl";

// Компиляция шейдера
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Проверка ошибок
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Ошибка компиляции шейдера:\n" << infoLog << std::endl;
    }
    return shader;
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Ошибка инициализации GLFW!" << std::endl;
        return -1;
    }

    // Настройка OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Создание окна
    GLFWwindow* window = glfwCreateWindow(600, 600, "Звезда с шейдером", nullptr, nullptr);
    if (!window) {
        std::cerr << "Ошибка создания окна!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Инициализация GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Ошибка инициализации GLEW!" << std::endl;
        return -1;
    }

    // Компиляция шейдеров
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Создание шейдерной программы
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Удаляем шейдеры (они уже в программе)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Координаты звезды (как в вашем коде)
    std::vector<float> vertices = { 0.0f, 0.0f };  // Центр

    const float PI = 3.14159265358979323846f;
    for (int i = 0; i <= 10; ++i) {
        float angle_deg = i * 36.0f + 90.0f;
        float angle_rad = angle_deg * PI / 180.0f;
        float radius = (i % 2 == 0) ? 0.5f : 0.2f;
        vertices.push_back(cos(angle_rad) * radius);
        vertices.push_back(sin(angle_rad) * radius);
    }

    // Создание VAO и VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Указываем атрибуты вершин
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Основной цикл
    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Белый фон
        glClear(GL_COLOR_BUFFER_BIT);

        // Используем шейдер
        glUseProgram(shaderProgram);

        // Устанавливаем красный цвет звезды (можно менять)
        GLint colorLoc = glGetUniformLocation(shaderProgram, "starColor");
        glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);  // RGB (красный)

        // Рисуем звезду
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, vertices.size() / 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Очистка
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}