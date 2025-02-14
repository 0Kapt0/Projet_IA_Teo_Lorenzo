// EnemyPatroller.hpp
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
    sf::VertexArray cone;

private:
    bool canSeePlayer(const Player& player, Grid& grid);
    float enemyAngle = 0.0f;
};

#endif
