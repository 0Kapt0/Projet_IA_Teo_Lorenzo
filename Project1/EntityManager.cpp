#include "EntityManager.hpp"

EntityManager::EntityManager() : player(nullptr) {}

void EntityManager::setPlayer(std::shared_ptr<Player> player) {
    this->player = player;
}

void EntityManager::addEnemy(std::shared_ptr<EnemyPatroller> enemy) {
    enemies.push_back(enemy);
}

std::vector<std::shared_ptr<EnemyPatroller>>& EntityManager::getEnemies() {
    return enemies;
}

void EntityManager::addDogo(std::shared_ptr<ChasingDogo> dogo) {
    dogos.push_back(dogo);
}

std::vector<std::shared_ptr<ChasingDogo>>& EntityManager::getDogos() {
    return dogos;
}

void EntityManager::addCamera(std::shared_ptr<CameraAI> camera) {
    cameras.push_back(camera);
}

std::vector<std::shared_ptr<CameraAI>>& EntityManager::getCameras() {
    return cameras;
}

// Ajout de la gestion des alliés
void EntityManager::addAlly(std::shared_ptr<AllyAI> ally) {
    allies.push_back(ally);
}

std::vector<std::shared_ptr<AllyAI>>& EntityManager::getAllies() {
    return allies;
}

void EntityManager::alertAllies(sf::Vector2f targetpos) {
    std::cout << "Alerting all allies at position: (" << targetpos.x << ", " << targetpos.y << ")\n";

    for (auto& ally : allies) {
        ally->alertAllies(targetpos);
    }
}

void EntityManager::update(float deltaTime, Grid& grid) {

    if (player) {
        player->update(deltaTime, grid);
    }

    for (auto& enemy : enemies) {
        enemy->update(deltaTime, grid, *player);
    }

    std::vector<Enemy*> enemyPointers;
    for (auto& enemy : enemies) {
        enemyPointers.push_back(enemy.get());
    }

    for (auto& dogo : dogos) {
        sf::Vector2i gridPos(
            static_cast<int>(dogo->getShape().getPosition().x / CELL_SIZE),
            static_cast<int>(dogo->getShape().getPosition().y / CELL_SIZE)
        );

        dogo->update(deltaTime, grid, *player, *this);

        dogo->computePathToPlayer(grid, player->getPosition());
    }

    for (auto& camera : cameras) {
        camera->update(deltaTime, grid, *player);
    }

    for (auto& ally : allies) {
        ally->updateA(deltaTime, grid, *player, enemyPointers);
    }
}

void EntityManager::draw(sf::RenderWindow& window, Grid& grid) {

    if (player) {
        window.draw(player->shape);
    }
    for (auto& enemy : enemies) {
        enemy->drawViewCone(window, grid);
        window.draw(enemy->shape);
    }
    for (auto& dogo : dogos) {
        window.draw(dogo->getShape());
    }
    for (auto& camera : cameras) {
        camera->draw(window, grid);
    }
    for (auto& ally : allies) {
        window.draw(ally->shape);
    }
}

std::shared_ptr<Player> EntityManager::getPlayer() const {
    return player;
}
