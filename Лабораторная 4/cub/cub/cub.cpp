#include <GL/freeglut.h>
#include <cmath>
#include <vector>

const float t = (1.0f + sqrt(5.0f)) / 2.0f;

float vertices[12][3] = {
    {-1.0f,  t, 0.0f}, {1.0f,  t, 0.0f}, {-1.0f, -t, 0.0f}, {1.0f, -t, 0.0f},
    {0.0f, -1.0f,  t}, {0.0f, 1.0f,  t}, {0.0f, -1.0f, -t}, {0.0f, 1.0f, -t},
    { t, 0.0f, -1.0f}, { t, 0.0f, 1.0f}, {-t, 0.0f, -1.0f}, {-t, 0.0f, 1.0f}
};

int faces[20][3] = {
    {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11},
    {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
    {3, 9, 4}, {3, 4, 2}, {3, 2, 6}, {3, 6, 8}, {3, 8, 9},
    {4, 9, 5}, {2, 4, 11}, {6, 2, 10}, {8, 6, 7}, {9, 8, 1}
};


float colors[10][3] = {
    {1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0},
    {1.0, 1.0, 0.0}, {0.0, 1.0, 1.0}, {1.0, 0.0, 1.0},
    {0.5, 0.5, 0.5}, {1.0, 0.5, 0.0}, {0.0, 0.5, 1.0},
    {1.0, 0.0, 0.5}
};

float angleX = 0, angleY = 0;
int lastX, lastY;
bool mouseLeftDown = false;

void drawIcosahedron() {
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < 20; i++) {
        glColor3fv(colors[i % 10]);
        for (int j = 0; j < 3; j++) {
            glVertex3fv(vertices[faces[i][j]]);
        }
    }
    glEnd();

    glColor3f(0, 0, 0);
    glBegin(GL_LINES);
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 3; j++) {
            glVertex3fv(vertices[faces[i][j]]);
            glVertex3fv(vertices[faces[i][(j + 1) % 3]]);
        }
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    drawIcosahedron();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mouseLeftDown = (state == GLUT_DOWN);
        lastX = x;
        lastY = y;
    }
}

void motion(int x, int y) {
    if (mouseLeftDown) {
        angleY += (x - lastX);
        angleX += (y - lastY);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float lightPos[4] = { 5.0, 5.0, 5.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Icosahedron");

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}