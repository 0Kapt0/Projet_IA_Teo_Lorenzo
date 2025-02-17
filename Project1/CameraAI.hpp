#ifndef CAMERA_AI_HPP
#define CAMERA_AI_HPP

#include "Enemy.hpp"
#include "Grid.hpp"
#include "Player.hpp"
#include "EntityManager.hpp"
#include <SFML/Graphics.hpp>

class EntityManager;

enum class CameraState { IDLE, ALERT, COOLDOWN };

class CameraAI : public Enemy {
public:
    CameraAI(float x, float y, EntityManager* manager);
    void update(float deltaTime, Grid& grid, Player& player);
    void draw(sf::RenderWindow& window, Grid& grid);
    bool isPlayerDetected(Player& player, Grid& grid);
    void alertEnemies(Vector2f targetpos);
    void desalertEnemies(Vector2f targetpos);
    sf::VertexArray getViewConeShape(Grid& grid);
    bool isTriangleIntersectingRect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::FloatRect rect);
    bool isPointInTriangle(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b, sf::Vector2f c);

private:
    Clock cooldownClock;
    float rotationSpeed = 20.0f;
    float detectionRange = 200.0f;
    float alertTimer = 0.0f;
    float cooldownTime = 7.5f;
    float cooldownTimeFreeze = 3.0f;
    CameraState state = CameraState::IDLE;
    EntityManager* entityManager;
};

#endif // CAMERA_AI_HPP

