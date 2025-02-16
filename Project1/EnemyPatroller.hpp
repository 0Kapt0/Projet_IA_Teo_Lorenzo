#ifndef ENEMY_PATROLLER_HPP
#define ENEMY_PATROLLER_HPP

#include "Enemy.hpp"
#include "Grid.hpp"
#include "Player.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace sf;
using namespace std;

enum class Goal {
    Patrolling,
    Chasing,
    LostIt,
    Searching
};

class EnemyPatroller : public Enemy {
public:
    float enemyAngle = 0.0f;
    EnemyPatroller(float x, float y);
    void update(float deltaTime, Grid& grid, Player& player);
    void drawViewCone(RenderWindow& window, Grid& grid);
    VertexArray getViewConeShape(Grid& grid);
    void rotateTowards(const Vector2f& direction);

    // State helpers
    bool atTargetPosition() const;
    void setAtTargetPosition(bool value);
    void reset();

    Vector2f targetpos;
    bool playerDetected = false;
    bool warning = false;
    float maxRotationSpeed = 90.0f;
    float deltaTime;

    void setWarning(bool alert);
private:
    bool atTarget = false;
    bool isPointInTriangle(Vector2f p, Vector2f a, Vector2f b, Vector2f c);
    bool isTriangleIntersectingRect(Vector2f a, Vector2f b, Vector2f c, FloatRect rect);

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

class GOAPPlanner {
public:
    vector<Action*> Plan(const EnemyPatroller& initialState, Goal goal);
};

#endif
