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
    void drawViewCone(sf::RenderWindow& window, Grid& grid);
    sf::VertexArray getViewConeShape(Grid& grid);

private:
    float enemyAngle = 0.0f;
    bool isPointInTriangle(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b, sf::Vector2f c);
    bool isTriangleIntersectingRect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::FloatRect rect);
};

#endif
