#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "Grid.hpp"

using namespace sf;
using namespace std;


class Entity {
public:
    RectangleShape shape;
    Vector2f velocity;

    Entity(float x, float y, Color color);
    virtual void update(float deltaTime, Grid& grid) = 0;
};



#endif // ENTITY_HPP