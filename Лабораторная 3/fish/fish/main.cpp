#include "aquarium.h"
#include <GL/glut.h>

Aquarium aquarium;
int windowWidth = 800;
int windowHeight = static_cast<int>(windowWidth / ASPECT_RATIO);
float lastTime = 0.0f;

void display();
void update();
void reshape(int width, int height);
//Не использовать глобальные переменные
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glutDisplayFunc(display);
    glutIdleFunc(update);
    glutReshapeFunc(reshape);

    lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    glutMainLoop();
    return 0;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    aquarium.draw();
    glutSwapBuffers();
}

void update() {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    aquarium.update(deltaTime);
    glutPostRedisplay();
}

void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = static_cast<float>(width) / height;
    if (aspect > ASPECT_RATIO) {
        float w = ASPECT_RATIO * height;
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        glViewport((width - w) / 2, 0, w, height);
    }
    else {
        float h = width / ASPECT_RATIO;
        glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        glViewport(0, (height - h) / 2, width, h);
    }

    glMatrixMode(GL_MODELVIEW);
}