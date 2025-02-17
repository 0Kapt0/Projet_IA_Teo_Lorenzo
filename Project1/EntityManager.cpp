#include "EntityManager.hpp"

EntityManager::EntityManager() : player(nullptr) {}

void EntityManager::setPlayer(shared_ptr<Player> player) {
    this->player = player;
}

void EntityManager::addEnemy(shared_ptr<EnemyPatroller> enemy) {
    enemies.push_back(enemy);
}

std::vector<std::shared_ptr<EnemyPatroller>>& EntityManager::getEnemies() {
    return enemies;
}


void EntityManager::addCamera(std::shared_ptr<CameraAI> camera) {
    cameras.push_back(camera);
}

std::vector<std::shared_ptr<CameraAI>>& EntityManager::getCameras() {
    return cameras;
}

void EntityManager::update(float deltaTime, Grid& grid) {

    if (player) {
        player->update(deltaTime, grid);
    }

    for (auto& enemy : enemies) {
        enemy->update(deltaTime, grid, *player);
    }

    for (auto& camera : cameras) {
        camera->update(deltaTime, grid, *player);
    }
}
void EntityManager::draw(RenderWindow& window, Grid& grid) {
    
    if (player) {
        window.draw(player->shape);
    }
    for (auto& enemy : enemies) {
        enemy->drawViewCone(window, grid);
        window.draw(enemy->shape);
    }
    for (auto& camera : cameras) {
        camera->draw(window, grid);
    }
}

shared_ptr<Player> EntityManager::getPlayer() const {
    return player;
}
