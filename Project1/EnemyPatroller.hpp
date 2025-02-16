#ifndef ENEMY_PATROLLER_HPP
#define ENEMY_PATROLLER_HPP

#include "Enemy.hpp"
#include "Grid.hpp"
#include "Player.hpp"
#include <SFML/Graphics.hpp>

class EnemyPatroller : public Enemy {
public:
    EnemyPatroller(float x, float y);
    void update(float deltaTime, Grid& grid, Player& player);
    void drawViewCone(RenderWindow& window, Grid& grid);
    VertexArray getViewConeShape(Grid& grid);

private:
    float enemyAngle = 0.0f;
    bool isPointInTriangle(Vector2f p, Vector2f a, Vector2f b, Vector2f c);
    bool isTriangleIntersectingRect(Vector2f a, Vector2f b, Vector2f c, FloatRect rect);
};

#endif
