#ifndef ENEMY_DOGO_HPP
#define ENEMY_DOGO_HPP

#include "Enemy.hpp"
#include "Player.hpp"
#include "Grid.hpp"
#include "EntityManager.hpp"
#include <vector>

class EntityManager;

class EnemyDogo : public Enemy {
public:
    EnemyDogo(float x, float y, EntityManager* manager);
    void update(float deltaTime, Grid& grid, Player& player, std::vector<Enemy*>& nearbyEnemies);
    void alertEnemies(Vector2f targetpos);
    void desalertEnemies(Vector2f targetpos);
    void reset();
    bool atTargetPosition() const;
    void setAtTargetPosition(bool value);
    void rotateTowards(const sf::Vector2f& direction);

protected:
    float deltaTime;
    float enemyAngle;
    float maxRotationSpeed = 90.0f;
    EntityManager* entityManager;

public:
    sf::Vector2f targetpos;
    sf::Vector2f lastKnownPosition;
    bool playerDetected;
    bool atTarget;
};

class ActionD {
public:
    virtual bool CanExecute(const EnemyDogo& state) = 0;
    virtual void Execute(EnemyDogo& state) = 0;
    virtual ~ActionD() {}
};

class ChasePlayerD : public ActionD {
public:
    bool CanExecute(const EnemyDogo& state) override;
    void Execute(EnemyDogo& state) override;
};

class GOAPPlannerD {
public:
    std::vector<ActionD*> Plan(const EnemyDogo& initialState, Goal goal);
};

#endif // ENEMY_DOGO_HPP
