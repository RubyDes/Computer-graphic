#include <GL/glut.h>
#include <cmath>

// Параметры поверхности (уменьшены в 2 раза)
float a = 0.5f, b = 0.25f;  // Было a=1.0f, b=0.5f
float range = 1.0f;          // Было 2.0f
int resolution = 30;

// Управление вращением
float angleX = 30.0f, angleY = 30.0f;
int lastX, lastY;
bool mouseLeftDown = false;

// Функция поверхности
float f(float x, float y) {
    return (x * x) / (a * a) - (y * y) / (b * b);
}

void init() {
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Освещение
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Материал поверхности
    GLfloat matDiffuse[] = { 0.7f, 0.7f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Камера ближе, так как фигура уменьшена
    gluLookAt(0, 0, 2.5f, 0, 0, 0, 0, 1, 0);

    glRotatef(angleX, 1, 0, 0);
    glRotatef(angleY, 0, 1, 0);

    // Дополнительное масштабирование для точного уменьшения
    glScalef(0.5f, 0.5f, 0.5f);

    // Рисуем сплошную поверхность
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);

    float step = 2 * range / resolution;

    for (int i = 0; i < resolution; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= resolution; j++) {
            float x1 = -range + i * step;
            float x2 = -range + (i + 1) * step;
            float y = -range + j * step;

            // Нормали
            float nx1 = 2 * x1 / (a * a);
            float ny1 = -2 * y / (b * b);
            float nz1 = -1.0f;
            float len1 = sqrt(nx1 * nx1 + ny1 * ny1 + nz1 * nz1);
            nx1 /= len1; ny1 /= len1; nz1 /= len1;

            float nx2 = 2 * x2 / (a * a);
            float ny2 = -2 * y / (b * b);
            float nz2 = -1.0f;
            float len2 = sqrt(nx2 * nx2 + ny2 * ny2 + nz2 * nz2);
            nx2 /= len2; ny2 /= len2; nz2 /= len2;

            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, y, f(x1, y));

            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y, f(x2, y));
        }
        glEnd();
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w / h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
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
        angleY += (x - lastX) * 0.5f;
        angleX += (y - lastY) * 0.5f;
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Уменьшенный гиперболический параболоид");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}