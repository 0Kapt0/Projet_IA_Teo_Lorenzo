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

class CameraAI;
class EnemyDogo;

class EntityManager {
public:
    EntityManager();

    void addCamera(shared_ptr<CameraAI> camera);
    vector<shared_ptr<CameraAI>>& getCameras();

    void setPlayer(shared_ptr<Player> player);
    void addEnemy(shared_ptr<EnemyPatroller> enemy);
    vector<shared_ptr<EnemyPatroller>>& getEnemies();

    void addDogo(shared_ptr<EnemyDogo> dogo);
    vector<shared_ptr<EnemyDogo>>& getDogos();

    void update(float deltaTime, Grid& grid);
    void draw(RenderWindow& window, Grid& grid);

    shared_ptr<Player> getPlayer() const;

private:
    shared_ptr<Player> player;
    vector<shared_ptr<EnemyPatroller>> enemies;
    vector<shared_ptr<EnemyDogo>> dogos;
    vector<shared_ptr<CameraAI>> cameras;
};

#endif // ENTITY_MANAGER_HPP