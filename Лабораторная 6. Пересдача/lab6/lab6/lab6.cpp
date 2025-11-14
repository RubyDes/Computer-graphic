#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <iostream>

// Простые структуры для 3D векторов и цветов
struct Vec3 {
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
};

struct Color {
    float r, g, b;
    Color(float r = 1.0f, float g = 1.0f, float b = 1.0f) : r(r), g(g), b(b) {}
};

class Ship {
public:
    Vec3 position;
    Vec3 velocity;
    bool isSinking;
    float sinkProgress;
    int type;
    Color color;

    Ship(Vec3 startPos, int shipType) : position(startPos), isSinking(false), sinkProgress(0.0f), type(shipType) {
        // Разная скорость для разных типов кораблей
        velocity = Vec3(-1.0f - (shipType * 0.5f), 0.0f, 0.0f);
        // Разные цвета для разных типов кораблей
        if (type == 0) color = Color(0.7f, 0.7f, 0.7f); // Серый - большой
        else if (type == 1) color = Color(0.5f, 0.5f, 0.8f); // Синий - средний
        else color = Color(0.8f, 0.5f, 0.5f); // Красный - маленький
    }

    void update(float deltaTime) {
        if (!isSinking) {
            position = position + velocity * deltaTime;
        }
        else {
            sinkProgress += deltaTime;
            position.y -= sinkProgress * 0.5f; // Погружение под воду
        }
    }

    void render() {
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);

        if (isSinking) {
            glRotatef(sinkProgress * 30.0f, 1.0f, 0.0f, 0.0f);
        }

        glColor3f(color.r, color.g, color.b);

        if (type == 0) { // Большой корабль
            // Корпус
            glBegin(GL_QUADS);
            // Низ
            glVertex3f(-2.0f, 0.0f, -0.5f);
            glVertex3f(2.0f, 0.0f, -0.5f);
            glVertex3f(2.0f, 0.0f, 0.5f);
            glVertex3f(-2.0f, 0.0f, 0.5f);

            // Верх
            glVertex3f(-1.5f, 0.5f, -0.4f);
            glVertex3f(1.5f, 0.5f, -0.4f);
            glVertex3f(1.5f, 0.5f, 0.4f);
            glVertex3f(-1.5f, 0.5f, 0.4f);

            // Борта
            glVertex3f(-2.0f, 0.0f, -0.5f);
            glVertex3f(2.0f, 0.0f, -0.5f);
            glVertex3f(1.5f, 0.5f, -0.4f);
            glVertex3f(-1.5f, 0.5f, -0.4f);

            glVertex3f(-2.0f, 0.0f, 0.5f);
            glVertex3f(2.0f, 0.0f, 0.5f);
            glVertex3f(1.5f, 0.5f, 0.4f);
            glVertex3f(-1.5f, 0.5f, 0.4f);
            glEnd();

            // Надстройка
            glColor3f(0.3f, 0.3f, 0.3f);
            glBegin(GL_QUADS);
            glVertex3f(-0.8f, 0.5f, -0.3f);
            glVertex3f(0.8f, 0.5f, -0.3f);
            glVertex3f(0.8f, 1.5f, -0.3f);
            glVertex3f(-0.8f, 1.5f, -0.3f);

            glVertex3f(-0.8f, 0.5f, 0.3f);
            glVertex3f(0.8f, 0.5f, 0.3f);
            glVertex3f(0.8f, 1.5f, 0.3f);
            glVertex3f(-0.8f, 1.5f, 0.3f);
            glEnd();

        }
        else if (type == 1) { // Средний корабль
            // Корпус
            glBegin(GL_QUADS);
            // Низ
            glVertex3f(-1.5f, 0.0f, -0.4f);
            glVertex3f(1.5f, 0.0f, -0.4f);
            glVertex3f(1.5f, 0.0f, 0.4f);
            glVertex3f(-1.5f, 0.0f, 0.4f);

            // Верх
            glVertex3f(-1.2f, 0.4f, -0.3f);
            glVertex3f(1.2f, 0.4f, -0.3f);
            glVertex3f(1.2f, 0.4f, 0.3f);
            glVertex3f(-1.2f, 0.4f, 0.3f);

            // Борта
            glVertex3f(-1.5f, 0.0f, -0.4f);
            glVertex3f(1.5f, 0.0f, -0.4f);
            glVertex3f(1.2f, 0.4f, -0.3f);
            glVertex3f(-1.2f, 0.4f, -0.3f);
            glEnd();

        }
        else { // Маленький корабль
            // Корпус
            glBegin(GL_QUADS);
            glVertex3f(-1.0f, 0.0f, -0.3f);
            glVertex3f(1.0f, 0.0f, -0.3f);
            glVertex3f(1.0f, 0.0f, 0.3f);
            glVertex3f(-1.0f, 0.0f, 0.3f);

            glVertex3f(-1.0f, 0.0f, -0.3f);
            glVertex3f(-0.7f, 0.3f, -0.2f);
            glVertex3f(0.7f, 0.3f, -0.2f);
            glVertex3f(1.0f, 0.0f, -0.3f);
            glEnd();
        }

        glPopMatrix();
    }

    void hit() {
        isSinking = true;
    }
};

class Torpedo {
public:
    Vec3 position;
    Vec3 velocity;
    bool active;

    Torpedo(Vec3 startPos, Vec3 direction) : position(startPos), active(true) {
        velocity = direction * 8.0f; // Скорость торпеды
    }

    void update(float deltaTime) {
        position = position + velocity * deltaTime;
        // Деактивировать если улетела далеко
        if (position.x > 50.0f || position.x < -50.0f ||
            position.y > 50.0f || position.y < -50.0f ||
            position.z > 50.0f || position.z < -50.0f) {
            active = false;
        }
    }

    void render() {
        glPushMatrix();
        glTranslatef(position.x, position.y, position.z);

        // Ориентация торпеды по направлению движения
        float angle = atan2(velocity.z, velocity.x);
        glRotatef(angle * 180.0f / 3.14159f, 0.0f, 1.0f, 0.0f);

        glColor3f(1.0f, 0.5f, 0.0f); // Оранжевый цвет

        // Тело торпеды
        glBegin(GL_QUADS);
        // Бока
        glVertex3f(0.0f, -0.05f, -0.05f);
        glVertex3f(0.3f, -0.05f, -0.05f);
        glVertex3f(0.3f, 0.05f, -0.05f);
        glVertex3f(0.0f, 0.05f, -0.05f);

        glVertex3f(0.0f, -0.05f, 0.05f);
        glVertex3f(0.3f, -0.05f, 0.05f);
        glVertex3f(0.3f, 0.05f, 0.05f);
        glVertex3f(0.0f, 0.05f, 0.05f);

        // Верх/низ
        glVertex3f(0.0f, -0.05f, -0.05f);
        glVertex3f(0.3f, -0.05f, -0.05f);
        glVertex3f(0.3f, -0.05f, 0.05f);
        glVertex3f(0.0f, -0.05f, 0.05f);

        glVertex3f(0.0f, 0.05f, -0.05f);
        glVertex3f(0.3f, 0.05f, -0.05f);
        glVertex3f(0.3f, 0.05f, 0.05f);
        glVertex3f(0.0f, 0.05f, 0.05f);
        glEnd();

        // Нос торпеды
        glColor3f(1.0f, 0.3f, 0.0f);
        glBegin(GL_TRIANGLES);
        glVertex3f(0.3f, -0.05f, -0.05f);
        glVertex3f(0.4f, 0.0f, 0.0f);
        glVertex3f(0.3f, 0.05f, -0.05f);

        glVertex3f(0.3f, 0.05f, -0.05f);
        glVertex3f(0.4f, 0.0f, 0.0f);
        glVertex3f(0.3f, 0.05f, 0.05f);

        glVertex3f(0.3f, 0.05f, 0.05f);
        glVertex3f(0.4f, 0.0f, 0.0f);
        glVertex3f(0.3f, -0.05f, 0.05f);

        glVertex3f(0.3f, -0.05f, 0.05f);
        glVertex3f(0.4f, 0.0f, 0.0f);
        glVertex3f(0.3f, -0.05f, -0.05f);
        glEnd();

        glPopMatrix();
    }
};

// Глобальные переменные
GLFWwindow* window;
std::vector<Ship> enemyShips;
std::vector<Torpedo> activeTorpedos;
int playerLives = 3;
float torpedoCooldown = 0.0f;
float shipSpawnTimer = 0.0f;
double lastMouseX = 0, lastMouseY = 0;
Vec3 cameraPos = Vec3(0.0f, 2.0f, 8.0f);
Vec3 cameraFront = Vec3(0.0f, 0.0f, -1.0f);
Vec3 cameraUp = Vec3(0.0f, 1.0f, 0.0f);

void setupProjection() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)width / (float)height, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
        cameraPos.x + cameraFront.x, cameraPos.y + cameraFront.y, cameraPos.z + cameraFront.z,
        cameraUp.x, cameraUp.y, cameraUp.z);
}

void initGame() {
    srand(time(NULL));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Простое освещение
    float lightPos[] = { 10.0f, 10.0f, 10.0f, 1.0f };
    float lightColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
}

void spawnShip() {
    float zPos = (rand() % 100) / 100.0f * 8.0f - 4.0f; // Случайная Z-позиция
    int type = rand() % 3; // Случайный тип корабля
    enemyShips.push_back(Ship(Vec3(15.0f, 0.0f, zPos), type));
}

void fireTorpedo() {
    if (torpedoCooldown <= 0.0f) {
        // Направление based on mouse position
        Vec3 direction = cameraFront;
        activeTorpedos.push_back(Torpedo(cameraPos, direction));
        torpedoCooldown = 1.5f; // КД 1.5 секунды
    }
}

void checkCollisions() {
    for (auto torpedoIt = activeTorpedos.begin(); torpedoIt != activeTorpedos.end();) {
        bool torpedoHit = false;

        for (auto shipIt = enemyShips.begin(); shipIt != enemyShips.end();) {
            if (!shipIt->isSinking) {
                float dx = torpedoIt->position.x - shipIt->position.x;
                float dy = torpedoIt->position.y - shipIt->position.y;
                float dz = torpedoIt->position.z - shipIt->position.z;
                float distance = sqrt(dx * dx + dy * dy + dz * dz);

                if (distance < 2.0f) { // Простое определение столкновения
                    shipIt->hit();
                    torpedoHit = true;
                    break;
                }
            }
            ++shipIt;
        }

        if (torpedoHit) {
            torpedoIt = activeTorpedos.erase(torpedoIt);
        }
        else {
            ++torpedoIt;
        }
    }
}

void updateGame(float deltaTime) {
    // Обновление кораблей
    for (auto it = enemyShips.begin(); it != enemyShips.end();) {
        it->update(deltaTime);

        // Удалить если корабль уплыл за экран или утонул
        if (it->position.x < -20.0f && !it->isSinking) {
            playerLives--;
            it = enemyShips.erase(it);
        }
        else if (it->isSinking && it->sinkProgress > 3.0f) {
            it = enemyShips.erase(it);
        }
        else {
            ++it;
        }
    }

    // Обновление торпед
    for (auto it = activeTorpedos.begin(); it != activeTorpedos.end();) {
        it->update(deltaTime);
        if (!it->active) {
            it = activeTorpedos.erase(it);
        }
        else {
            ++it;
        }
    }

    // Спавн новых кораблей
    shipSpawnTimer += deltaTime;
    if (shipSpawnTimer > 3.0f) {
        spawnShip();
        shipSpawnTimer = 0.0f;
    }

    // Обновление КД торпеды
    if (torpedoCooldown > 0.0f) {
        torpedoCooldown -= deltaTime;
    }

    checkCollisions();
}

void renderGame() {
    glClearColor(0.2f, 0.3f, 0.8f, 1.0f); // Синий фон (море/небо)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setupProjection();

    // Рендер моря
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.2f, 0.6f);
    glVertex3f(-50.0f, -1.0f, -50.0f);
    glVertex3f(50.0f, -1.0f, -50.0f);
    glVertex3f(50.0f, -1.0f, 50.0f);
    glVertex3f(-50.0f, -1.0f, 50.0f);
    glEnd();
    glEnable(GL_LIGHTING);

    // Рендер кораблей
    for (auto& ship : enemyShips) {
        ship.render();
    }

    // Рендер торпед
    for (auto& torpedo : activeTorpedos) {
        torpedo.render();
    }

    // Отображение жизней
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 800, 0, 600, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 0.0f, 0.0f);
    for (int i = 0; i < playerLives; i++) {
        glBegin(GL_QUADS);
        glVertex2f(20 + i * 30, 560);
        glVertex2f(40 + i * 30, 560);
        glVertex2f(40 + i * 30, 580);
        glVertex2f(20 + i * 30, 580);
        glEnd();
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;
    lastMouseX = xpos;
    lastMouseY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    static float yaw = -90.0f;
    static float pitch = 0.0f;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    cameraFront.x = cos(yaw * 3.14159f / 180.0f) * cos(pitch * 3.14159f / 180.0f);
    cameraFront.y = sin(pitch * 3.14159f / 180.0f);
    cameraFront.z = sin(yaw * 3.14159f / 180.0f) * cos(pitch * 3.14159f / 180.0f);

    // Нормализация
    float length = sqrt(cameraFront.x * cameraFront.x +
        cameraFront.y * cameraFront.y +
        cameraFront.z * cameraFront.z);
    cameraFront.x /= length;
    cameraFront.y /= length;
    cameraFront.z /= length;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        fireTorpedo();
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main() {
    // Инициализация GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    window = glfwCreateWindow(800, 600, "Морской бой 3D", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    initGame();

    float lastTime = glfwGetTime();

    // Игровой цикл
    while (!glfwWindowShouldClose(window) && playerLives > 0) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        updateGame(deltaTime);
        renderGame();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Конец игры
    if (playerLives <= 0) {
        std::cout << "Игра окончена! Вы проиграли." << std::endl;
    }

    glfwTerminate();
    return 0;
}