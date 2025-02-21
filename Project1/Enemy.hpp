#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Entity.hpp"

class Enemy : public Entity {
public:
    float SPEED = 100.0f;
    Enemy(float x, float y);
    void update(float deltaTime, Grid& grid) override;
};
#endif // ENEMY_HPP