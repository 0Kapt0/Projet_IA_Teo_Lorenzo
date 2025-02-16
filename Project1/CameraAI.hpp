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
    void alertEnemies();
    sf::VertexArray getViewConeShape(Grid& grid);
    bool isTriangleIntersectingRect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::FloatRect rect);
    bool isPointInTriangle(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b, sf::Vector2f c); // Added function declaration

private:
    float rotationSpeed = 50.0f; // Degrees per second
    float detectionRange = 300.0f;
    float alertTimer = 0.0f;
    float cooldownTime = 3.0f;
    CameraState state = CameraState::IDLE;
    EntityManager* entityManager;
};

#endif // CAMERA_AI_HPP

