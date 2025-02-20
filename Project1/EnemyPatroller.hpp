// EnemyPatroller.hpp
#ifndef ENEMY_PATROLLER_HPP
#define ENEMY_PATROLLER_HPP

#include "Enemy.hpp"
#include "Grid.hpp"
#include "Player.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <queue>
#include <map>

using namespace sf;
using namespace std;

enum class Goal {
    Patrolling,
    Chasing,
    LostIt,
    Searching
};

struct Vector2iComparator {
    bool operator()(const Vector2i& a, const Vector2i& b) const {
        return std::tie(a.x, a.y) < std::tie(b.x, b.y);
    }
};

class EnemyPatroller : public Enemy {
public:
    float enemyAngle = 0.0f;
    EnemyPatroller(float x, float y, Vector2f point1, Vector2f point2, Vector2f point3);
    void update(float deltaTime, Grid& grid, Player& player);
    void drawViewCone(RenderWindow& window, Grid& grid);
    VertexArray getViewConeShape(Grid& grid);
    void rotateTowards(const Vector2f& direction);

    // State helpers
    bool atTargetPosition() const;
    void setAtTargetPosition(bool value);
    void Patrolling();
    void reset();

    float lookAroundTime = 0.0f;
    int rotatedir =1;
    Vector2f targetpos;
    bool playerDetected = false;
    bool warning = false;
    float maxRotationSpeed = 90.0f;
    float deltaTime;
    void computePathToPlayer(Grid& grid, const Vector2f& targetPos);
    void setWarning(bool alert, Vector2f targetpos);
private:
    void moveTowardsTarget(float deltaTime);
    std::queue<Vector2f> pathToPlayer;
    float speed = 160.0f;
    std::vector<Vector2i> debugPath;

    Texture enemyTexture;
    int etape = 1;
    bool atTarget = false;
    bool isPointInTriangle(Vector2f p, Vector2f a, Vector2f b, Vector2f c);
    bool isTriangleIntersectingRect(Vector2f a, Vector2f b, Vector2f c, FloatRect rect);
    Vector2f point1;
    Vector2f point2;
    Vector2f point3;

};

// ================================================================================Actions================================================================================
class Action {
public:
    virtual bool CanExecute(const EnemyPatroller& state) = 0;
    virtual void Execute(EnemyPatroller& state) = 0;
    virtual ~Action() {}
};

class ChasePlayer : public Action {
public:
    bool CanExecute(const EnemyPatroller& state) override;
    void Execute(EnemyPatroller& state) override;
};

class LookAround : public Action {
public:
    bool CanExecute(const EnemyPatroller& state) override;
    void Execute(EnemyPatroller& state) override;
};

class Patrol : public Action {
public:
    bool CanExecute(const EnemyPatroller& state) override;
    void Execute(EnemyPatroller& state) override;
};

class GOAPPlanner {
public:
    vector<Action*> Plan(const EnemyPatroller& initialState, Goal Goal);
};

#endif
