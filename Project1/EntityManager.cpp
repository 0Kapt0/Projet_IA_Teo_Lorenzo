#include "EntityManager.hpp"

EntityManager::EntityManager() : player(nullptr) {}

void EntityManager::setPlayer(shared_ptr<Player> player) {
    this->player = player;
}

void EntityManager::addEnemy(shared_ptr<EnemyPatroller> enemy) {
    enemies.push_back(enemy);
}

vector<shared_ptr<EnemyPatroller>>& EntityManager::getEnemies() {
    return enemies;
}

void EntityManager::addDogo(shared_ptr<EnemyDogo> dogo) {
    dogos.push_back(dogo);
}

vector<shared_ptr<EnemyDogo>>& EntityManager::getDogos() {
    return dogos;
}

void EntityManager::addCamera(shared_ptr<CameraAI> camera) {
    cameras.push_back(camera);
}

vector<shared_ptr<CameraAI>>& EntityManager::getCameras() {
    return cameras;
}

void EntityManager::update(float deltaTime, Grid& grid) {

    if (player) {
        player->update(deltaTime, grid);
    }

    for (auto& enemy : enemies) {
        enemy->update(deltaTime, grid, *player);
    }

    vector<Enemy*> enemyPointers;
    for (auto& enemy : enemies) {
        enemyPointers.push_back(enemy.get());
    }

    for (auto& dogo : dogos) {
        dogo->update(deltaTime, grid, *player, enemyPointers);
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
    for (auto& dogo : dogos) {
        window.draw(dogo->shape);
    }
    for (auto& camera : cameras) {
        camera->draw(window, grid);
    }
}

shared_ptr<Player> EntityManager::getPlayer() const {
    return player;
}
