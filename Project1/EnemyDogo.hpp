#ifndef CHASING_DOGO_HPP
#define CHASING_DOGO_HPP

#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <map>
#include "EnemyPatroller.hpp"
#include "EntityManager.hpp"

class Grid;
class Player;
class EntityManager;

struct Vector2iComparator {
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const {
        return std::tie(a.x, a.y) < std::tie(b.x, b.y);
    }
};

class ChasingDogo {
public:
    ChasingDogo(float x, float y);
    void update(float deltaTime, Grid& grid, Player& player, EntityManager& entityManager);
    void draw(sf::RenderWindow& window, Grid& grid);
    void computePathToPlayer(Grid& grid, const sf::Vector2f& targetPos);

    sf::RectangleShape getShape() const { return shape; }

private:
    void moveTowardsTarget(float deltaTime);
    float adjustSpeedForTurn(sf::Vector2f currentPos, float moveStep);
    void checkCollisionWithPlayer(Player& player, EntityManager& entityManager);

    sf::RectangleShape shape;
    std::queue<sf::Vector2f> pathToPlayer;
    float speed = 160.0f;
    std::vector<sf::Vector2i> debugPath;

    sf::Vector2f lastPosition;
    sf::Vector2f lastRunningPosition;
    sf::Vector2f lastKnownPlayerPosition; // Dernière position connue du joueur quand il sprintait
    bool playerWasRunning = false; // Indique si le joueur était en train de sprinter avant
    int stuckCounter = 0;
    int frameCounter = 0;
    Texture dogoTexture;
};

#endif // CHASING_DOGO_HPP
