#include <GL/glut.h>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cmath>

const int ROWS = 4;
const int COLS = 4;
const float CARD_WIDTH = 1.5f;
const float CARD_HEIGHT = 2.0f;
const float CARD_THICKNESS = 0.1f;  // Обьемность карточек
const float SPACING = 0.2f; // расстояние между карточками
const float FLIP_SPEED = 5.0f; // скорость поворота карточки

enum CardState {
    HIDDEN,
    FLIPPING,
    REVEALED,
    MATCHED,
    FLIPPING_BACK
};

struct Card {
    int value;              
    CardState state;
    float flipAngle;
    float x, y;
    bool flipped;
};

std::vector<Card> cards;
int firstFlippedIndex = -1;
int secondFlippedIndex = -1;
bool gameActive = true;
bool animationInProgress = false;

const float cardColors[8][3] = {
    {0.165f, 0.588f, 0.878f},
    {0.953f, 0.612f, 0.071f},
    {0.153f, 0.682f, 0.376f},
    {0.678f, 0.063f, 0.235f},
    {0.902f, 0.494f, 0.133f},
    {0.557f, 0.267f, 0.678f},
    {0.827f, 0.329f, 0.000f},
    {0.180f, 0.800f, 0.251f}
};

void initCards() {
    cards.clear();

    // создаем пару значений для карт
    std::vector<int> values;
    for (int i = 0; i < ROWS * COLS / 2; i++) {
        values.push_back(i);
        values.push_back(i);
    }

    std::shuffle(values.begin(), values.end(), std::mt19937(std::random_device()()));

    // создание карт и размещение на поле
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            Card card;
            card.value = values[row * COLS + col];
            card.state = HIDDEN;
            card.flipAngle = 0.0f;
            card.flipped = false;
            card.x = col * (CARD_WIDTH + SPACING) - (COLS * (CARD_WIDTH + SPACING)) / 2 + CARD_WIDTH / 2;
            card.y = -row * (CARD_HEIGHT + SPACING) + (ROWS * (CARD_HEIGHT + SPACING)) / 2 - CARD_HEIGHT / 2;
            cards.push_back(card);
        }
    }
}

void drawCardSymbol(int value) {
    const float* color = cardColors[value % 8];
    glColor3fv(color);

    switch (value % 4) {
    case 0:
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= 360; i += 10) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(cos(angle) * 0.4f, sin(angle) * 0.4f);
        }
        glEnd();
        break;

    case 1:
        glBegin(GL_QUADS);
        glVertex2f(-0.4f, -0.4f);
        glVertex2f(0.4f, -0.4f);
        glVertex2f(0.4f, 0.4f);
        glVertex2f(-0.4f, 0.4f);
        glEnd();
        break;

    case 2:
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.4f);
        glVertex2f(-0.4f, -0.4f);
        glVertex2f(0.4f, -0.4f);
        glEnd();
        break;

    case 3:
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.0f, 0.0f);
        for (int i = 0; i <= 10; i++) {
            float angle_deg = i * 36.0f + 90.0f;
            float angle_rad = angle_deg * 3.14159f / 180.0f;
            float radius = (i % 2 == 0) ? 0.4f : 0.2f;
            glVertex2f(cos(angle_rad) * radius, sin(angle_rad) * radius);
        }
        glEnd();
        break;
    }

}

void init() {
    glClearColor(0.933f, 0.933f, 0.933f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat lightPos[] = { 5.0f, 5.0f, 10.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    initCards();
}

void drawCard(const Card& card) {
    glPushMatrix();
    glTranslatef(card.x, card.y, 0);

    glRotatef(card.flipAngle, 0, 1, 0);

    glEnable(GL_CULL_FACE);

    if (card.flipAngle <= 90) {
        glCullFace(GL_BACK);
        glColor3f(0.5f, 0.5f, 0.5f);

        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glEnd();

        glColor3f(0.3f, 0.3f, 0.3f);

        // Top side
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glEnd();

        // Bottom side
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glEnd();

        // Left side
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glEnd();

        // Right side
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glEnd();
    }
    else {
        glCullFace(GL_FRONT);
        glColor3f(1.0f, 1.0f, 1.0f);  // Белый фон

        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glEnd();

        glPushMatrix();
        glTranslatef(0, 0, -CARD_THICKNESS / 2 - 0.01f);
        drawCardSymbol(card.value);
        glPopMatrix();

        glColor3f(0.7f, 0.7f, 0.7f);

        // Top side
        glBegin(GL_QUADS);
        glNormal3f(0, 1, 0);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glEnd();

        // Bottom side
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glEnd();

        // Left side
        glBegin(GL_QUADS);
        glNormal3f(-1, 0, 0);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(-CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glEnd();

        // Right side
        glBegin(GL_QUADS);
        glNormal3f(1, 0, 0);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, -CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, -CARD_THICKNESS / 2);
        glVertex3f(CARD_WIDTH / 2, CARD_HEIGHT / 2, CARD_THICKNESS / 2);
        glEnd();
    }

    glDisable(GL_CULL_FACE);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);

    for (const auto& card : cards) {
        if (card.state != MATCHED) {
            drawCard(card);
        }
    }

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int findClickedCard(int mouseX, int mouseY) {
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLdouble posX, posY, posZ;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    GLfloat winY = (float)viewport[3] - (float)mouseY;
    GLfloat winZ;
    glReadPixels(mouseX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    gluUnProject((GLdouble)mouseX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    for (size_t i = 0; i < cards.size(); i++) {
        if (cards[i].state == MATCHED) continue;

        float left = cards[i].x - CARD_WIDTH / 2;
        float right = cards[i].x + CARD_WIDTH / 2;
        float top = cards[i].y + CARD_HEIGHT / 2;
        float bottom = cards[i].y - CARD_HEIGHT / 2;

        if (posX >= left && posX <= right && posY >= bottom && posY <= top) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void checkMatch() {
    if (cards[firstFlippedIndex].value == cards[secondFlippedIndex].value) {
        cards[firstFlippedIndex].state = MATCHED;
        cards[secondFlippedIndex].state = MATCHED;

        bool allMatched = true;
        for (const auto& card : cards) {
            if (card.state != MATCHED) {
                allMatched = false;
                break;
            }
        }

        if (allMatched) {
            gameActive = false;
            printf("Congratulations! You found all pairs!\n");
        }
    }
    else {
        cards[firstFlippedIndex].state = FLIPPING_BACK;
        cards[secondFlippedIndex].state = FLIPPING_BACK;
    }

    firstFlippedIndex = -1;
    secondFlippedIndex = -1;
}

void mouse(int button, int state, int x, int y) {
    if (!gameActive || animationInProgress) return;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int cardIndex = findClickedCard(x, y);
        if (cardIndex != -1 && cardIndex < static_cast<int>(cards.size())) {
            Card& card = cards[cardIndex];

            if (card.state == HIDDEN) {
                if (firstFlippedIndex == -1) {
                    card.state = FLIPPING;
                    card.flipped = true;
                    firstFlippedIndex = cardIndex;
                    animationInProgress = true;
                }
                else if (secondFlippedIndex == -1 && cardIndex != firstFlippedIndex) {
                    card.state = FLIPPING;
                    card.flipped = true;
                    secondFlippedIndex = cardIndex;
                    animationInProgress = true;
                }
            }
            else if (card.state == REVEALED) {
                if (firstFlippedIndex == cardIndex) {
                    card.state = FLIPPING_BACK;
                    firstFlippedIndex = -1;
                    animationInProgress = true;
                }
                else if (secondFlippedIndex == cardIndex) {
                    card.state = FLIPPING_BACK;
                    secondFlippedIndex = -1;
                    animationInProgress = true;
                }
                else if (firstFlippedIndex != -1 && secondFlippedIndex == -1) {
                    cards[firstFlippedIndex].state = FLIPPING_BACK;
                    card.state = FLIPPING_BACK;
                    firstFlippedIndex = -1;
                    animationInProgress = true;
                }
            }
        }
    }
}

void update(int value) {
    bool stillAnimating = false;

    for (size_t i = 0; i < cards.size(); i++) {
        Card& card = cards[i];

        switch (card.state) {
        case FLIPPING:
            card.flipAngle += FLIP_SPEED;
            if (card.flipAngle >= 180) {
                card.flipAngle = 180;
                card.state = REVEALED;
                if (secondFlippedIndex != -1) {
                    checkMatch();
                }
            }
            else {
                stillAnimating = true;
            }
            break;

        case FLIPPING_BACK:
            card.flipAngle -= FLIP_SPEED;
            if (card.flipAngle <= 0) {
                card.flipAngle = 0;
                card.state = HIDDEN;
                card.flipped = false;
            }
            else {
                stillAnimating = true;
            }
            break;

        default:
            break;
        }
    }

    animationInProgress = stillAnimating;
    glutPostRedisplay();

    if (gameActive || animationInProgress) {
        glutTimerFunc(16, update, 0);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Memory Trainer");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}