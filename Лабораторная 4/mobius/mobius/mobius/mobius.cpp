#include <GL/glut.h>
#include <cmath>

const float ScaleFactor = 1.5f;

class MobiusStrip {
public:
    void Draw() {
        glBegin(GL_QUAD_STRIP);
        glColor3f(0.8f, 0.2f, 0.4f);

        double u, v;
        for (v = -1; v <= 1; v += 0.05) {
            for (u = 0; u < 2 * 3.14 + 0.05; u += 0.05) {
                SetVertex(u, v);
                SetVertex(u, v + 0.05);
            }
        }
        glEnd();
    }

private:
    void SetVertex(double u, double v) {
        float x = (float)((1 + v / 2 * cos(u / 2)) * cos(u) * ScaleFactor);
        float y = (float)((1 + v / 2 * cos(u / 2)) * sin(u) * ScaleFactor);
        float z = (float)(v / 2 * sin(u / 2)) * ScaleFactor;
        glVertex3f(x, y, z);
    }
};

MobiusStrip mobius;
float angleX = 0, angleY = 0;
int lastX, lastY;
bool mouseLeftDown = false;

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    mobius.Draw();
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        mouseLeftDown = (state == GLUT_DOWN);
        lastX = x; lastY = y;
    }
}

void motion(int x, int y) {
    if (mouseLeftDown) {
        angleY += (x - lastX) * 0.5f;
        angleX += (y - lastY) * 0.5f;
        lastX = x; lastY = y;
        glutPostRedisplay();
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Лента Мёбиуса");

    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}