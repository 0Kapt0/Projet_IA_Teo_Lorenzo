#ifndef COOKIE_HPP
#define COOKIE_HPP

#include "Entity.hpp"

class Cookie : public Entity {
public:

    Cookie(float x, float y);
    void update(float deltaTime, Grid& grid) override;
};

#endif