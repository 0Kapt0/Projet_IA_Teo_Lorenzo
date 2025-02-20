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
    bool operator()(const Vector2i& a, const Vector2i& b) const {
        return std::tie(a.x, a.y) < std::tie(b.x, b.y);
    }
};

class ChasingDogo {
public:
    ChasingDogo(float x, float y);
    void update(float deltaTime, Grid& grid, Player& player, EntityManager& entityManager);
    void draw(RenderWindow& window, Grid& grid);
    void computePathToPlayer(Grid& grid, const Vector2f& targetPos);

    RectangleShape getShape() const { return shape; }

private:
    void moveTowardsTarget(float deltaTime);
    float adjustSpeedForTurn(Vector2f currentPos, float moveStep);
    void checkCollisionWithPlayer(Player& player, EntityManager& entityManager);

    RectangleShape shape;
    std::queue<Vector2f> pathToPlayer;
    float speed = 160.0f;
    std::vector<Vector2i> debugPath;

    Vector2f lastPosition;
    Vector2f lastRunningPosition;
    int stuckCounter = 0;
    int frameCounter = 0;
    Texture dogoTexture;
};

#endif // CHASING_DOGO_HPP
