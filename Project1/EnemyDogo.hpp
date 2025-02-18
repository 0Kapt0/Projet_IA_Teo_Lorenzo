#ifndef ENEMY_DOGO_HPP
#define ENEMY_DOGO_HPP

#include "Enemy.hpp"
#include "Player.hpp"
#include "Grid.hpp"
#include "EntityManager.hpp"
#include <vector>
#include <map>
#include <queue>

class EntityManager;

// 📌 Comparator pour utiliser sf::Vector2i dans std::map et std::set
struct Vector2iComparator {
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const {
        return std::tie(a.x, a.y) < std::tie(b.x, b.y);
    }
};


class EnemyDogo : public Enemy {
public:
    EnemyDogo(float x, float y, EntityManager* manager);
    void update(float deltaTime, Grid& grid, Player& player, std::vector<Enemy*>& nearbyEnemies);
    void alertEnemies(sf::Vector2f targetpos);
    void desalertEnemies(sf::Vector2f targetpos);
    void reset();
    bool atTargetPosition() const;
    void setAtTargetPosition(bool value);
    void rotateTowards(const sf::Vector2f& direction);
    void computePathToPlayer();  // Recalcule un chemin valide

protected:
    float enemyAngle;
    float maxRotationSpeed = 90.0f;

public:
    EntityManager* entityManager;
    sf::Vector2f targetpos;
    sf::Vector2f lastKnownPosition;
    sf::Texture dogoTexture;
    bool playerDetected;
    bool atTarget;
    float deltaTime;
    std::queue<sf::Vector2f> pathToPlayer;
    EntityManager* getEntityManager() const { return entityManager; }

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
    std::vector<ActionD*> Plan(const EnemyDogo& initialState, Goal Goal);
};

#endif // ENEMY_DOGO_HPP
