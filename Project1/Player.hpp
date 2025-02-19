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
    vector<Cookie> cookies;
    sf::Vector2f getPosition() const { return shape.getPosition(); }
private:
    bool isRunning = false;
};

#endif // PLAYER_HPP