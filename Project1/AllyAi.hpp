#ifndef ALLY_AI_HPP
#define ALLY_AI_HPP

#include "Entity.hpp"
#include "Player.hpp"
#include "Grid.hpp"
#include "EntityManager.hpp"
#include <vector>

class EntityManager;

enum class GoalA {
    Patrolling,
    Distract,
    Protect,
    Block
};

class AllyAI : public Entity {
public:
    AllyAI(float x, float y, EntityManager* manager);
    void update(float deltaTime, Grid& grid) override;
    void updateA(float deltaTime, Grid& grid, Player& player, std::vector<Enemy*>& nearbyEnemies);
    void alertAllies(Vector2f targetpos);
    void reset();
    bool atTargetPosition() const;
    void setAtTargetPosition(bool value);
    void rotateTowards(const sf::Vector2f& direction);
    void throwDiversion(Vector2f pos);
    void openHidingSpot(Vector2f pos);
    void blockEnemyPath(Vector2f pos);
    void computePathToPlayerA();

protected:
    float deltaTime;
    float allyAngle;
    float maxRotationSpeed = 90.0f;
    EntityManager* entityManager;

public:
    sf::Vector2f targetpos;
    sf::Vector2f lastKnownPosition;
    bool playerDetected;
    bool atTarget;
};

class ActionAlly {
public:
    virtual bool CanExecute(const AllyAI& state) = 0;
    virtual void Execute(AllyAI& state) = 0;
    virtual ~ActionAlly() {}
};

class ThrowDiversion : public ActionAlly {
public:
    bool CanExecute(const AllyAI& state) override;
    void Execute(AllyAI& state) override;
};

class OpenHidingSpot : public ActionAlly {
public:
    bool CanExecute(const AllyAI& state) override;
    void Execute(AllyAI& state) override;
};

class BlockEnemyPath : public ActionAlly {
public:
    bool CanExecute(const AllyAI& state) override;
    void Execute(AllyAI& state) override;
};

class GOAPPlannerAlly {
public:
    std::vector<ActionAlly*> Plan(const AllyAI& initialState, GoalA Goal);
};

#endif // ALLY_AI_HPP
