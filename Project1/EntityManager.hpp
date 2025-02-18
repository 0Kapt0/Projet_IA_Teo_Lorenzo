#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <vector>
#include <memory>
#include "Entity.hpp"
#include "Player.hpp"
#include "EnemyPatroller.hpp"
#include "Grid.hpp"
#include "CameraAI.hpp"
#include "EnemyDogo.hpp"
#include "AllyAi.hpp"

class CameraAI;
class EnemyDogo;
class AllyAI;

class EntityManager {
public:
    EntityManager();

    void setPlayer(std::shared_ptr<Player> player);
    void addEnemy(std::shared_ptr<EnemyPatroller> enemy);
    std::vector<std::shared_ptr<EnemyPatroller>>& getEnemies();

    void addDogo(std::shared_ptr<EnemyDogo> dogo);
    std::vector<std::shared_ptr<EnemyDogo>>& getDogos();

    void addCamera(std::shared_ptr<CameraAI> camera);
    std::vector<std::shared_ptr<CameraAI>>& getCameras();

    void addAlly(std::shared_ptr<AllyAI> ally);
    std::vector<std::shared_ptr<AllyAI>>& getAllies();

    void alertAllies(sf::Vector2f targetpos);
    void update(float deltaTime, Grid& grid);
    void draw(sf::RenderWindow& window, Grid& grid);

    std::shared_ptr<Player> getPlayer() const;
    Grid& getGrid() { return grid; }

private:
    std::shared_ptr<Player> player;
    std::vector<std::shared_ptr<EnemyPatroller>> enemies;
    std::vector<std::shared_ptr<EnemyDogo>> dogos;
    std::vector<std::shared_ptr<CameraAI>> cameras;
    std::vector<std::shared_ptr<AllyAI>> allies;
    Grid grid;
};

#endif // ENTITY_MANAGER_HPP
