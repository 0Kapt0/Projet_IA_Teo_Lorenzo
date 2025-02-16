#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <vector>
#include <memory>
#include "Entity.hpp"
#include "Player.hpp"
#include "EnemyPatroller.hpp"
#include "Grid.hpp"

class EntityManager {
public:
    EntityManager();

    void setPlayer(shared_ptr<Player> player);
    void addEnemy(shared_ptr<EnemyPatroller> enemy);

    void update(float deltaTime, Grid& grid);
    void draw(RenderWindow& window, Grid& grid);

    shared_ptr<Player> getPlayer() const;

private:
    shared_ptr<Player> player;
    vector<shared_ptr<EnemyPatroller>> enemies;
};

#endif // ENTITY_MANAGER_HPP
