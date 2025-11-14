#include "aquarium.h"
#include <cmath>
#include <ctime>
#include <cstdlib>

Fish::Fish(float x, float y, float size, float speed, float r, float g, float b)
    : x(x), y(y), size(size), speed(speed), direction(1.0f){
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void Fish::update(float deltaTime) {
    x += speed * direction * deltaTime;
    if (x > 0.9f) direction = -1.0f;
    if (x < -0.9f) direction = 1.0f;
}

FishType1::FishType1(float x, float y, float size, float speed)
    : Fish(x, y, size, speed, 1.0f, 0.5f, 0.0f) {}

void FishType1::draw(float time) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    if (direction < 0) glScalef(-1.0f, 1.0f, 1.0f);

    glColor3fv(color);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f);
    const int segments = 36;
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        glVertex2f(size * cos(angle),           
            size * 0.5f * sin(angle));
    }
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(-size * 0.8f, 0.0f);
    glVertex2f(-size * 1.5f, size * 0.5f);
    glVertex2f(-size * 1.5f, -size * 0.5f);
    glEnd();

    glPopMatrix();
}

FishType2::FishType2(float x, float y, float size, float speed)
    : Fish(x, y, size, speed, 0.0f, 0.5f, 1.0f) {}

void FishType2::draw(float time) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    if (direction < 0) glScalef(-1.0f, 1.0f, 1.0f);

    glColor3fv(color);
    glBegin(GL_POLYGON);
    glVertex2f(size * 0.8f, 0.0f);
    glVertex2f(0.0f, size * 0.5f);
    glVertex2f(-size * 0.8f, size * 0.3f);
    glVertex2f(-size * 0.8f, -size * 0.3f);
    glVertex2f(0.0f, -size * 0.5f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(-size * 0.3f, size * 0.3f);
    glVertex2f(-size * 0.1f, size * 0.7f);
    glVertex2f(size * 0.1f, size * 0.3f);
    glVertex2f(-size * 0.3f, -size * 0.3f);
    glVertex2f(-size * 0.1f, -size * 0.7f);
    glVertex2f(size * 0.1f, -size * 0.3f);
    glEnd();

    glPopMatrix();
}

FishType3::FishType3(float x, float y, float size, float speed)
    : Fish(x, y, size, speed, 1.0f, 0.0f, 0.5f) {}

void FishType3::draw(float time) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    if (direction < 0) glScalef(-1.0f, 1.0f, 1.0f);

    glColor3fv(color);
    glBegin(GL_POLYGON);
    glVertex2f(size, 0.0f);
    glVertex2f(0.0f, size * 0.4f);
    glVertex2f(-size * 0.5f, size * 0.2f);
    glVertex2f(-size, 0.0f);
    glVertex2f(-size * 0.5f, -size * 0.2f);
    glVertex2f(0.0f, -size * 0.4f);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex2f(-size, 0.0f);
    glVertex2f(-size * 1.5f, size * 0.3f);
    glVertex2f(-size * 1.5f, -size * 0.3f);
    glEnd();

    glPopMatrix();
}

Plant::Plant(float x, float height, float width, float r, float g, float b)
    : x(x), height(height), width(width) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void Plant::draw() {
    glPushMatrix();
    glTranslatef(x, -0.9f, 0.0f);
    glColor3fv(color);
    glBegin(GL_TRIANGLE_STRIP);
    for (float y = 0; y <= height; y += height / 10.0f) {
        float w = width * (1.0f - y / (height * 1.5f));
        float offset = sin(y * 5.0f) * width * 0.2f;
        glVertex2f(offset - w, y);
        glVertex2f(offset + w, y);
    }
    glEnd();
    glPopMatrix();
}

Stone::Stone(float x, float y, float size, float r, float g, float b)
    : x(x), y(y), size(size) {
    color[0] = r;
    color[1] = g;
    color[2] = b;
}

void Stone::draw() {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glColor3fv(color);

    glBegin(GL_POLYGON);
    glVertex2f(-size * 0.8f, -size * 0.5f);
    glVertex2f(-size * 0.3f, -size * 0.9f);
    glVertex2f(size * 0.2f, -size * 0.7f);
    glVertex2f(size * 0.7f, -size * 0.3f);
    glVertex2f(size * 0.5f, size * 0.1f);
    glVertex2f(size * 0.1f, size * 0.4f);
    glVertex2f(-size * 0.5f, size * 0.2f);
    glEnd();

    glPopMatrix();
}

Aquarium::Aquarium() : time(0.0f) {

    //Использовать умные показатели, например unic

    fishes.push_back(new FishType1(0.0f, 0.0f, 0.1f, 0.3f));
    fishes.push_back(new FishType3(0.5f, -0.1f, 0.12f, 0.25f));
    fishes.push_back(new FishType2(-0.2f, -0.2f, 0.09f, 0.35f));

    plants.push_back(Plant(-0.8f, 0.5f, 0.05f, 0.0f, 0.7f, 0.0f));
    plants.push_back(Plant(-0.5f, 0.7f, 0.07f, 0.1f, 0.8f, 0.1f));
    plants.push_back(Plant(-0.2f, 0.6f, 0.06f, 0.0f, 0.6f, 0.0f));
    plants.push_back(Plant(0.1f, 0.4f, 0.04f, 0.2f, 0.7f, 0.2f));
    plants.push_back(Plant(0.4f, 0.8f, 0.08f, 0.0f, 0.8f, 0.0f));
    plants.push_back(Plant(0.7f, 0.5f, 0.05f, 0.1f, 0.6f, 0.1f));

    stones.push_back(Stone(-0.9f, -0.85f, 0.08f, 0.5f, 0.5f, 0.5f));
    stones.push_back(Stone(-0.7f, -0.9f, 0.12f, 0.6f, 0.4f, 0.2f));
    stones.push_back(Stone(-0.4f, -0.87f, 0.1f, 0.3f, 0.3f, 0.3f));
    stones.push_back(Stone(-0.1f, -0.88f, 0.09f, 0.7f, 0.7f, 0.6f));
    stones.push_back(Stone(0.2f, -0.85f, 0.07f, 0.4f, 0.4f, 0.5f));
    stones.push_back(Stone(0.5f, -0.9f, 0.11f, 0.5f, 0.6f, 0.5f));
    stones.push_back(Stone(0.8f, -0.86f, 0.06f, 0.8f, 0.7f, 0.6f));
}

Aquarium::~Aquarium() {
    for (auto fish : fishes) {
        delete fish;
    }
}

void Aquarium::update(float deltaTime) {
    time += deltaTime;
    for (auto fish : fishes) {
        fish->update(deltaTime);
    }
}

void Aquarium::draw() {
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.4f, 0.8f);
    glVertex2f(-1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glColor3f(0.1f, 0.2f, 0.5f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.9f, 0.8f, 0.6f);
    glVertex2f(-1.0f, -0.9f);
    glVertex2f(1.0f, -0.9f);
    glColor3f(0.7f, 0.6f, 0.4f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glEnd();

    for (auto& plant : plants) {
        plant.draw();
    }

    for (auto& stone : stones) {
        stone.draw();
    }

    for (auto fish : fishes) {
        fish->draw(time);
    }

}