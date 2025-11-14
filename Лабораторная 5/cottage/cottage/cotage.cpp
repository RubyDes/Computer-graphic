#include <windows.h>
#include <GL/glut.h>
#include <gdiplus.h>
#include <cmath>
#include <iostream>
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "freeglut.lib")

using namespace Gdiplus;

float angleX = 0.0f;
float angleY = 0.0f;
int lastX = 0, lastY = 0;
bool mouseLeftDown = false;

GLuint brickTexture, roofTexture, grassTexture, woodTexture, windowTexture, doorTexture, garageTexture, garageDoor;

ULONG_PTR gdiplusToken;
GdiplusStartupInput gdiplusStartupInput;

#ifndef GL_BGRA
#define GL_BGRA 0x80E1
#endif

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

GLuint LoadTexture(const wchar_t* filename) {
    Bitmap bmp(filename);
    if (bmp.GetLastStatus() != Ok) {
        std::wcerr << L"Failed to load texture: " << filename << std::endl;
        return 0;
    }

    Bitmap* convertedBmp = nullptr;
    if (bmp.GetPixelFormat() != PixelFormat32bppARGB) {
        convertedBmp = new Bitmap(bmp.GetWidth(), bmp.GetHeight(), PixelFormat32bppARGB);
        Graphics g(convertedBmp);
        g.DrawImage(&bmp, 0, 0, bmp.GetWidth(), bmp.GetHeight());
    }

    Bitmap* finalBmp = convertedBmp ? convertedBmp : &bmp;

    Rect rect(0, 0, finalBmp->GetWidth(), finalBmp->GetHeight());
    BitmapData bitmapData;
    finalBmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, finalBmp->GetWidth(), finalBmp->GetHeight(),
        0, GL_BGRA, GL_UNSIGNED_BYTE, bitmapData.Scan0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    finalBmp->UnlockBits(&bitmapData);
    if (convertedBmp) delete convertedBmp;

    return texture;
}

void loadTextures() {
    brickTexture = LoadTexture(L"bricks.png");
    roofTexture = LoadTexture(L"roof.png");
    grassTexture = LoadTexture(L"grass.png");
    woodTexture = LoadTexture(L"bricks.png");
    windowTexture = LoadTexture(L"glass.png");
    doorTexture = LoadTexture(L"door.png");
    garageTexture = LoadTexture(L"brick.png");
    garageDoor = LoadTexture(L"garage_door.png");
}

class TexturedCube {
public:
    void draw(float x, float y, float z, float width, float height, float depth, GLuint texture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glColor3f(1.0f, 1.0f, 1.0f);

        glPushMatrix();
        glTranslatef(x, y, z);

        //Перед
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, -height / 2, depth / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, -height / 2, depth / 2);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(width / 2, height / 2, depth / 2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-width / 2, height / 2, depth / 2);
        glEnd();

        //Задняя часть
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-width / 2, -height / 2, -depth / 2);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-width / 2, height / 2, -depth / 2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(width / 2, height / 2, -depth / 2);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(width / 2, -height / 2, -depth / 2);
        glEnd();

        // Верх
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-width / 2, height / 2, -depth / 2);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, height / 2, depth / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, height / 2, depth / 2);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(width / 2, height / 2, -depth / 2);
        glEnd();

        // Низ
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-width / 2, -height / 2, -depth / 2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(width / 2, -height / 2, -depth / 2);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(width / 2, -height / 2, depth / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-width / 2, -height / 2, depth / 2);
        glEnd();

        // Левая грань
        glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-width / 2, -height / 2, -depth / 2);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(-width / 2, height / 2, -depth / 2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-width / 2, height / 2, depth / 2);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, -height / 2, depth / 2);
        glEnd();

        // Правая грань
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(width / 2, -height / 2, -depth / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, -height / 2, depth / 2);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(width / 2, height / 2, depth / 2);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(width / 2, height / 2, -depth / 2);
        glEnd();

        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    }
};

class Roof {
public:
    void draw(float x, float y, float z, float width, float height, float depth, GLuint texture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glColor3f(1.0f, 1.0f, 1.0f);

        glPushMatrix();
        glTranslatef(x, y, z);

        // Передняя грань
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0, height / 2, depth / 2);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, -height / 2, depth / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, -height / 2, depth / 2);
        glEnd();

        // Задняя грань
        glBegin(GL_TRIANGLES);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0, height / 2, -depth / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, -height / 2, -depth / 2);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, -height / 2, -depth / 2);
        glEnd();

        // Боковые грани
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, -height / 2, depth / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(-width / 2, -height / 2, -depth / 2);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0, height / 2, -depth / 2);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0, height / 2, depth / 2);

        glTexCoord2f(0.0f, 0.0f); glVertex3f(width / 2, -height / 2, depth / 2);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0, height / 2, depth / 2);
        glTexCoord2f(0.5f, 1.0f); glVertex3f(0, height / 2, -depth / 2);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, -height / 2, -depth / 2);
        glEnd();

        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
    }
};

class Fence {
public:
    void draw(float x, float y, float z, float dx, float dz, int segments, GLuint texture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture);
        glColor3f(1.0f, 1.0f, 1.0f);

        float segmentLength = (dx != 0) ? dx / segments : dz / segments;

        for (int i = 0; i <= segments; i++) {
            float px = x + (dx != 0 ? i * segmentLength : 0);
            float pz = z + (dz != 0 ? i * segmentLength : 0);

            glPushMatrix();
            glTranslatef(px, y + 0.5f, pz);
            glScalef(0.1f, 1.0f, 0.1f);
            glutSolidCube(1.0f);
            glPopMatrix();
        }

        // Перекладины
        for (int j = 0; j < 2; j++) {
            float h = y + 0.3f + j * 0.4f;
            for (int i = 0; i < segments; i++) {
                float px = x + (dx != 0 ? i * segmentLength : 0);
                float pz = z + (dz != 0 ? i * segmentLength : 0);
                float nx = x + (dx != 0 ? (i + 1) * segmentLength : 0);
                float nz = z + (dz != 0 ? (i + 1) * segmentLength : 0);

                glPushMatrix();
                glTranslatef((px + nx) / 2, h, (pz + nz) / 2);
                glScalef(dx != 0 ? segmentLength : 0.05f, 0.05f, dz != 0 ? segmentLength : 0.05f);
                glutSolidCube(1.0f);
                glPopMatrix();
            }
        }
        glDisable(GL_TEXTURE_2D);
    }
};


class Cottage {
private:
    TexturedCube cube;
    Roof roof;
public:
    void draw() {
        // Основное здание
        cube.draw(0.0f, 0.0f, 0.0f, 3.0f, 2.0f, 4.0f, brickTexture);

        // Крыша
        roof.draw(0.0f, 1.5f, 0.0f, 3.5f, 1.2f, 4.5f, roofTexture);

        // Окна
        cube.draw(-1.0f, 0.3f, 2.01f, 0.8f, 0.8f, 0.1f, windowTexture);
        cube.draw(1.0f, 0.3f, 2.01f, 0.8f, 0.8f, 0.1f, windowTexture);

        // Дверь
        cube.draw(0.0f, -0.3f, 2.01f, 0.8f, 1.4f, 0.1f, doorTexture);
    }
};

class Garage {
private:
    TexturedCube cube;
public:
    void draw() {
        // Основное здание
        cube.draw(3.0f, -0.1f, 0.0f, 3.0f, 2.0f, 2.5f, garageTexture);

        // Крыша
        cube.draw(3.0f, 0.9f, 0.0f, 3.0f, 0.1f, 2.5f, roofTexture);

        // Дверь гаража
        cube.draw(3.0f, -0.1f, -1.26f, 2.0f, 1.8f, 0.1f, garageDoor);

        // Окно
        cube.draw(3.0f, 0.0f, 1.26f, 0.8f, 0.8f, 0.1f, windowTexture);

    }
};

void init() {
    glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Отключаем освещение
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    glShadeModel(GL_FLAT);

    // Загрузка текстур
    loadTextures();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(10.0f * cos(angleY) * cos(angleX),
        10.0f * sin(angleX),
        10.0f * sin(angleY) * cos(angleX),
        0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, -1.0f, -20.0f);
    glTexCoord2f(10.0f, 0.0f); glVertex3f(20.0f, -1.0f, -20.0f);
    glTexCoord2f(10.0f, 10.0f); glVertex3f(20.0f, -1.0f, 20.0f);
    glTexCoord2f(0.0f, 10.0f); glVertex3f(-20.0f, -1.0f, 20.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    Cottage cottage;
    cottage.draw();

    Garage garage;
    garage.draw();

    Fence fence;

    float fx = -4.0f, fz = -3.5f, fw = 11.0f, fd = 7.0f;
    int segW = 22, segD = 14;
    // Передняя сторона
    fence.draw(fx, -1.0f, fz, fw, 0.0f, segW, woodTexture);
    // Задняя сторона
    fence.draw(fx, -1.0f, fz + fd, fw, 0.0f, segW, woodTexture);
    // Левая сторона
    fence.draw(fx, -1.0f, fz, 0.0f, fd, segD, woodTexture);
    // Правая сторона
    fence.draw(fx + fw, -1.0f, fz, 0.0f, fd, segD, woodTexture);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseLeftDown = true;
            lastX = x;
            lastY = y;
        }
        else if (state == GLUT_UP) {
            mouseLeftDown = false;
        }
    }
}

void motion(int x, int y) {
    if (mouseLeftDown) {
        angleY += (x - lastX) * 0.01f;
        angleX += (y - lastY) * 0.01f;

        lastX = x;
        lastY = y;

        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Cottage");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();

    GdiplusShutdown(gdiplusToken);
    return 0;
}