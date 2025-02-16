#include "EntityManager.hpp"

EntityManager::EntityManager() : player(nullptr) {}

void EntityManager::setPlayer(shared_ptr<Player> player) {
    this->player = player;
}

void EntityManager::addEnemy(shared_ptr<EnemyPatroller> enemy) {
    enemies.push_back(enemy);
}

void EntityManager::update(float deltaTime, Grid& grid) {

    if (player) {
        player->update(deltaTime, grid);
    }

    for (auto& enemy : enemies) {
        enemy->update(deltaTime, grid, *player);
    }
}
void EntityManager::draw(RenderWindow& window, Grid& grid) {
    
    if (player) {
        window.draw(player->shape);
    }
    for (auto& enemy : enemies) {
        window.draw(enemy->shape);
        enemy->drawViewCone(window, grid);
    }
}

shared_ptr<Player> EntityManager::getPlayer() const {
    return player;
}
