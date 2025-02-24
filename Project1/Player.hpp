﻿#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Entity.hpp"
#include "Cookie.hpp"

class Player : public Entity {
public:

    float SPEED = 200.0f;
    Player(float x, float y);
    void update(float deltaTime, Grid& grid) override;
    bool getIsRunning();
    void cookiedraw(RenderWindow& window);
    vector<unique_ptr<Cookie>> cookies;    
    Vector2f getPosition() const { return shape.getPosition(); }
    RectangleShape getShape() const { return shape; }
private:
    Texture cookietexture;
    bool isRunning = false;
};

#endif // PLAYER_HPP