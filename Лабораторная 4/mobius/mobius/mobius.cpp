#include <GL/freeglut.h>
#include <cmath>
#include <vector>

const float ScaleFactor = 1.5f;

class MobiusStrip {
public:
    void Draw() {
        glBegin(GL_QUAD_STRIP);

        double u, v;
        for (v = -1; v <= 1; v += 0.05) {
            for (u = 0; u < 2 * M_PI + 0.05; u += 0.05) {
                SetVertex(u, v);
                SetVertex(u, v + 0.05);
            }
        }

        glEnd();
    }

private:
    void SetColorByCoords(float x, float y, float z) {
        float len = sqrt(x * x + y * y + z * z);
        if (len > 0) {
            x /= len; y /= len; z /= len;
        }

        glColor3f(fabs(x), fabs(y), fabs(z));
    }

    void SetVertex(double u, double v) {
        float x = (float)((1 + v / 2 * cos(u / 2)) * cos(u)) * ScaleFactor;
        float y = (float)((1 + v / 2 * cos(u / 2)) * sin(u)) * ScaleFactor;
        float z = (float)(v / 2 * sin(u / 2)) * ScaleFactor;

        SetColorByCoords(x, y, z);
        glVertex3f(x, y, z);
    }
};

// Глобальные переменные
MobiusStrip mobius;
float rotateX = 0, rotateY = 0;
int lastX, lastY;
bool mouseLeftDown = false;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

    glRotatef(rotateX, 1, 0, 0);
    glRotatef(rotateY, 0, 1, 0);

    mobius.Draw();

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
        rotateY += (x - lastX) * 0.5f;
        rotateX += (y - lastY) * 0.5f;
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double aspect = (double)w / h;
    double frustumSize = 2.0;
    double frustumHeight = frustumSize;
    double frustumWidth = frustumHeight * aspect;

    if (frustumWidth < frustumSize && aspect != 0) {
        frustumWidth = frustumSize;
        frustumHeight = frustumWidth / aspect;
    }

    glFrustum(
        -frustumWidth * 0.5, frustumWidth * 0.5,
        -frustumHeight * 0.5, frustumHeight * 0.5,
        frustumSize * 0.5, frustumSize * 30.0
    );

    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Настройка освещения
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    float lightPos[] = { 10.0f, 10.0f, 10.0f, 0.0f };
    float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    float lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    float lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void specialKeys(int key, int x, int y) {
    // Обработка специальных клавиш (по желанию)
    switch (key) {
    case GLUT_KEY_UP: rotateX -= 5; break;
    case GLUT_KEY_DOWN: rotateX += 5; break;
    case GLUT_KEY_LEFT: rotateY -= 5; break;
    case GLUT_KEY_RIGHT: rotateY += 5; break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Лента Мёбиуса (FreeGLUT)");

    init();

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    glutSpecialFunc(specialKeys); // Для обработки стрелок

    glutMainLoop();
    return 0;
}