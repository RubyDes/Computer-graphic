#pragma once
#ifndef AQUARIUM_H
#define AQUARIUM_H

#include <vector>
#include <GL/glut.h>

const float ASPECT_RATIO = 16.0f / 10.0f;

class Fish {
protected:
    float x, y;
    float size;
    float speed;
    float direction;
    float color[3];

public:
    Fish(float x, float y, float size, float speed, float r, float g, float b);
    virtual ~Fish() {}
    virtual void draw(float time) = 0;
    void update(float deltaTime);
};

class FishType1 : public Fish {
public:
    FishType1(float x, float y, float size, float speed);
    void draw(float time) override;
};

class FishType2 : public Fish {
public:
    FishType2(float x, float y, float size, float speed);
    void draw(float time) override;
};

class FishType3 : public Fish {
public:
    FishType3(float x, float y, float size, float speed);
    void draw(float time) override;
};

class Plant {
private:
    float x;
    float height;
    float width;
    float color[3];

public:
    Plant(float x, float height, float width, float r, float g, float b);
    void draw();
};

class Stone {
private:
    float x, y;
    float size;
    float color[3];

public:
    Stone(float x, float y, float size, float r, float g, float b);
    void draw();
};

//Оформить по соглашению форматирования на c++ на Гитхаб
//Неизменяемые методы должны быть константными 

class Aquarium {
private:
    std::vector<Fish*> fishes;
    std::vector<Plant> plants;
    std::vector<Stone> stones;
    float time;

public:
    Aquarium();
    ~Aquarium();
    void update(float deltaTime);
    void draw();
};

#endif