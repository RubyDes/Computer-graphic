#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <omp.h>
#include <fstream>

// Вектор в 3D пространстве
struct Vector3 {
    double x, y, z;

    Vector3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(double scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    double dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vector3 normalize() const {
        double len = length();
        if (len == 0) return *this;
        return Vector3(x / len, y / len, z / len);
    }
};

// Цвет (RGB)
struct Color {
    double r, g, b;

    Color(double r = 0, double g = 0, double b = 0) : r(r), g(g), b(b) {}

    Color operator+(const Color& other) const {
        return Color(r + other.r, g + other.g, b + other.b);
    }

    Color operator*(double scalar) const {
        return Color(r * scalar, g * scalar, b * scalar);
    }

    Color operator*(const Color& other) const {
        return Color(r * other.r, g * other.g, b * other.b);
    }

    // Ограничение значений цвета до [0, 1]
    Color clamp() const {
        return Color(
            std::max(0.0, std::min(1.0, r)),
            std::max(0.0, std::min(1.0, g)),
            std::max(0.0, std::min(1.0, b))
        );
    }
};

// Материал поверхности
struct Material {
    Color diffuse;      // Цвет диффузного отражения
    Color specular;     // Цвет зеркального отражения
    Color ambient;      // Цвет фонового отражения
    double shininess;   // Степень зеркального блеска

    // Конструктор по умолчанию
    Material() : diffuse(Color(0.5, 0.5, 0.5)), specular(Color(1.0, 1.0, 1.0)),
        ambient(Color(0.1, 0.1, 0.1)), shininess(32.0) {}

    // Конструктор с параметрами
    Material(const Color& diff, const Color& spec, const Color& amb, double shine)
        : diffuse(diff), specular(spec), ambient(amb), shininess(shine) {}
};

// Источник света
struct Light {
    Vector3 position;
    Color diffuse;      // Цвет диффузного излучения
    Color specular;     // Цвет зеркального излучения
    Color ambient;      // Цвет фонового излучения

    Light(const Vector3& pos, const Color& diff, const Color& spec, const Color& amb)
        : position(pos), diffuse(diff), specular(spec), ambient(amb) {}
};

// Луч (для проверки пересечений)
struct Ray {
    Vector3 origin;
    Vector3 direction;

    Ray(const Vector3& orig, const Vector3& dir) : origin(orig), direction(dir.normalize()) {}

    Vector3 pointAt(double t) const {
        return origin + direction * t;
    }
};

// Поверхность (сфера для примера)
struct Sphere {
    Vector3 center;
    double radius;
    Material material;

    Sphere(const Vector3& c, double r, const Material& mat) : center(c), radius(r), material(mat) {}

    // Проверка пересечения луча со сферой
    bool intersect(const Ray& ray, double& t) const {
        Vector3 oc = ray.origin - center;
        double a = ray.direction.dot(ray.direction);
        double b = 2.0 * oc.dot(ray.direction);
        double c = oc.dot(oc) - radius * radius;
        double discriminant = b * b - 4 * a * c;

        if (discriminant < 0) return false;

        double sqrtDisc = std::sqrt(discriminant);
        double t1 = (-b - sqrtDisc) / (2.0 * a);
        double t2 = (-b + sqrtDisc) / (2.0 * a);

        t = (t1 > 0) ? t1 : t2;
        return t > 0;
    }

    // Получение нормали в точке на поверхности
    Vector3 getNormal(const Vector3& point) const {
        return (point - center).normalize();
    }
};

// Сцена
class Scene {
private:
    std::vector<Sphere> objects;
    std::vector<Light> lights;

public:
    void addObject(const Sphere& object) {
        objects.push_back(object);
    }

    void addLight(const Light& light) {
        lights.push_back(light);
    }

    // Проверка, находится ли точка в тени относительно источника света
    bool isInShadow(const Vector3& point, const Light& light) const {
        Vector3 lightDir = (light.position - point).normalize();
        Ray shadowRay(point + lightDir * 0.001, lightDir); // Смещение для избежания самопересечения

        for (const auto& obj : objects) {
            double t;
            if (obj.intersect(shadowRay, t)) {
                double distanceToLight = (light.position - point).length();
                if (t < distanceToLight) {
                    return true; // Есть препятствие на пути к свету
                }
            }
        }
        return false;
    }

    // Расчет цвета в точке с учетом освещения Фонга и теней
    Color calculateColor(const Vector3& point, const Vector3& normal, const Vector3& viewDir, const Material& material) const {
        Color result(0, 0, 0);

        for (const auto& light : lights) {
            Vector3 lightDir = (light.position - point).normalize();

            // Фоновая составляющая (всегда присутствует)
            Color ambient = material.ambient * light.ambient;
            result = result + ambient;

            // Проверка на наличие тени
            if (isInShadow(point, light)) {
                continue; // Пропускаем диффузную и зеркальную составляющие для этого источника
            }

            // Диффузная составляющая
            double diff = std::max(0.0, normal.dot(lightDir));
            Color diffuse = material.diffuse * light.diffuse * diff;
            result = result + diffuse;

            // Зеркальная составляющая (модель Фонга)
            Vector3 reflectDir = (lightDir * (-1.0) + normal * (2.0 * normal.dot(lightDir))).normalize();
            double spec = std::pow(std::max(0.0, reflectDir.dot(viewDir)), material.shininess);
            Color specular = material.specular * light.specular * spec;
            result = result + specular;
        }

        return result.clamp();
    }

    // Поиск ближайшего пересечения луча с объектами сцены
    bool findClosestIntersection(const Ray& ray, Vector3& hitPoint, Vector3& normal, Material& material) const {
        double closestT = std::numeric_limits<double>::max();
        bool found = false;

        for (const auto& obj : objects) {
            double t;
            if (obj.intersect(ray, t) && t < closestT) {
                closestT = t;
                hitPoint = ray.pointAt(t);
                normal = obj.getNormal(hitPoint);
                material = obj.material;
                found = true;
            }
        }

        return found;
    }
};

// Класс для рендеринга с использованием OpenMP
class ParallelRaycaster {
private:
    Scene scene;
    int width, height;
    std::vector<Color> imageBuffer;

public:
    ParallelRaycaster(int w, int h) : width(w), height(h) {
        imageBuffer.resize(width * height);
        setupScene();
    }

    void setupScene() {
        // Добавляем материалы
        Material redMaterial(Color(0.8, 0.2, 0.2), Color(1.0, 1.0, 1.0), Color(0.1, 0.0, 0.0), 32.0);
        Material greenMaterial(Color(0.2, 0.8, 0.2), Color(1.0, 1.0, 1.0), Color(0.0, 0.1, 0.0), 64.0);
        Material blueMaterial(Color(0.2, 0.2, 0.8), Color(1.0, 1.0, 1.0), Color(0.0, 0.0, 0.1), 128.0);
        Material floorMaterial(Color(0.7, 0.7, 0.7), Color(0.3, 0.3, 0.3), Color(0.05, 0.05, 0.05), 16.0);

        // Добавляем объекты
        scene.addObject(Sphere(Vector3(0, 0, -5), 1.0, redMaterial));
        scene.addObject(Sphere(Vector3(-2, 0, -7), 1.5, greenMaterial));
        scene.addObject(Sphere(Vector3(2, 0, -6), 1.2, blueMaterial));
        scene.addObject(Sphere(Vector3(0, -1002, 0), 1000.0, floorMaterial)); // Пол

        // Добавляем источники света
        Light mainLight(Vector3(3, 5, -3), Color(0.8, 0.8, 0.8), Color(1.0, 1.0, 1.0), Color(0.1, 0.1, 0.1));
        Light fillLight(Vector3(-3, 2, -2), Color(0.4, 0.4, 0.4), Color(0.5, 0.5, 0.5), Color(0.05, 0.05, 0.05));

        scene.addLight(mainLight);
        scene.addLight(fillLight);
    }

    // Трассировка луча для каждого пикселя
    Color traceRay(int x, int y) {
        // Преобразование координат пикселя в нормализованные координаты сцены
        double ndcX = (x + 0.5) / width * 2.0 - 1.0;
        double ndcY = 1.0 - (y + 0.5) / height * 2.0;

        // Положение камеры
        Vector3 cameraPos(0, 0, 0);
        Vector3 rayDir(ndcX, ndcY, -1);
        Ray ray(cameraPos, rayDir);

        Vector3 hitPoint, normal;
        Material material;

        if (scene.findClosestIntersection(ray, hitPoint, normal, material)) {
            Vector3 viewDir = (cameraPos - hitPoint).normalize();
            return scene.calculateColor(hitPoint, normal, viewDir, material);
        }

        // Цвет фона, если пересечений нет
        return Color(0.1, 0.1, 0.3);
    }

    // Параллельный рендеринг сцены с использованием OpenMP
    void renderParallel() {
        double startTime = omp_get_wtime();

        std::cout << "Starting parallel render with " << omp_get_max_threads() << " threads..." << std::endl;

#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < width * height; ++i) {
            int x = i % width;
            int y = i / width;
            imageBuffer[i] = traceRay(x, y);

            // Вывод прогресса
            if (i % (width * 10) == 0) {
#pragma omp critical
                {
                    double progress = (i * 100.0) / (width * height);
                    std::cout << "Progress: " << progress << "%" << std::endl;
                }
            }
        }

        double endTime = omp_get_wtime();
        std::cout << "Render completed in " << (endTime - startTime) << " seconds" << std::endl;
    }

    // Сохранение изображения в формате PPM
    void saveToPPM(const std::string& filename) {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return;
        }

        // Заголовок PPM
        file << "P3\n" << width << " " << height << "\n255\n";

        // Данные изображения
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Color& color = imageBuffer[y * width + x];
                int r = static_cast<int>(color.r * 255);
                int g = static_cast<int>(color.g * 255);
                int b = static_cast<int>(color.b * 255);

                file << r << " " << g << " " << b << " ";
            }
            file << "\n";
        }

        file.close();
        std::cout << "Image saved to: " << filename << std::endl;
    }

    // Сохранение изображения в формате BMP (простая реализация)
    void saveToBMP(const std::string& filename) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            return;
        }

        // Заголовок BMP
        const int fileSize = 54 + 3 * width * height;
        const int dataOffset = 54;

        // BITMAPFILEHEADER
        unsigned char fileHeader[14] = {
            'B', 'M',           // Сигнатура
            fileSize & 0xFF, (fileSize >> 8) & 0xFF, (fileSize >> 16) & 0xFF, (fileSize >> 24) & 0xFF, // Размер файла
            0, 0, 0, 0,         // Зарезервировано
            dataOffset & 0xFF, (dataOffset >> 8) & 0xFF, 0, 0 // Смещение данных
        };

        // BITMAPINFOHEADER
        unsigned char infoHeader[40] = {
            40, 0, 0, 0,        // Размер заголовка
            width & 0xFF, (width >> 8) & 0xFF, (width >> 16) & 0xFF, (width >> 24) & 0xFF, // Ширина
            height & 0xFF, (height >> 8) & 0xFF, (height >> 16) & 0xFF, (height >> 24) & 0xFF, // Высота
            1, 0,               // Количество плоскостей
            24, 0,              // Бит на пиксель
            0, 0, 0, 0,         // Сжатие
            0, 0, 0, 0,         // Размер изображения
            0, 0, 0, 0,         // Разрешение по горизонтали
            0, 0, 0, 0,         // Разрешение по вертикали
            0, 0, 0, 0,         // Количество цветов в палитре
            0, 0, 0, 0          // Количество важных цветов
        };

        file.write(reinterpret_cast<char*>(fileHeader), 14);
        file.write(reinterpret_cast<char*>(infoHeader), 40);

        // Данные изображения (BGR)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const Color& color = imageBuffer[y * width + x];
                unsigned char b = static_cast<unsigned char>(color.b * 255);
                unsigned char g = static_cast<unsigned char>(color.g * 255);
                unsigned char r = static_cast<unsigned char>(color.r * 255);

                file.put(b);
                file.put(g);
                file.put(r);
            }
            // Выравнивание до 4 байт
            for (int i = 0; i < (4 - (width * 3) % 4) % 4; ++i) {
                file.put(0);
            }
        }

        file.close();
        std::cout << "BMP image saved to: " << filename << std::endl;
    }
};

int main() {
    std::cout << "Raycaster with Phong Lighting and Shadows" << std::endl;
    std::cout << "=========================================" << std::endl;

    // Настройка размеров изображения
    int width = 800;
    int height = 600;

    // Создаем рейкастер
    ParallelRaycaster raycaster(width, height);

    // Рендерим сцену
    raycaster.renderParallel();

    // Сохраняем результат
    raycaster.saveToPPM("output.ppm");
    raycaster.saveToBMP("output.bmp");

    std::cout << "Done!" << std::endl;

    return 0;
}