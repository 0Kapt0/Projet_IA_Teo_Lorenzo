#include "EnemyPatroller.hpp"
#include <cmath>

EnemyPatroller::EnemyPatroller(float x, float y) : Enemy(x, y) {
    shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);
}

void EnemyPatroller::update(float deltaTime, Grid& grid, Player& player) {
    enemyAngle += 1;
    shape.setRotation(enemyAngle);
    if (canSeePlayer(player, grid)) {
        shape.setFillColor(sf::Color::Green);
    }
    else {
        shape.setFillColor(sf::Color::Red);
    }
}

bool EnemyPatroller::canSeePlayer(const Player& player, Grid& grid) {
    sf::Vector2f enemyPos = shape.getPosition();
    sf::Vector2f playerPos = player.shape.getPosition();
    sf::Vector2f direction = playerPos - enemyPos;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 200.0f) return false;

    direction /= distance;

    float enemyAngle = 90.0f;
    sf::Vector2f enemyForward(std::cos(enemyAngle), std::sin(enemyAngle));

    float dotProduct = direction.x * enemyForward.x + direction.y * enemyForward.y;
    float angleBetween = std::acos(dotProduct) * (180.0f / 3.14159265358979323846);

    if (angleBetween > 25.0f) return false;

    for (float i = 0; i < distance; i += 5.0f) {
        sf::Vector2f checkPos = enemyPos + direction * i;
        int gridX = static_cast<int>(checkPos.x / CELL_SIZE);
        int gridY = static_cast<int>(checkPos.y / CELL_SIZE);
        if (!grid.getCell(gridX, gridY).walkable) {
            return false;
        }
    }
    return true;
}

void EnemyPatroller::drawViewCone(sf::RenderWindow& window, Grid& grid) {
    cone.clear();
    cone.setPrimitiveType(sf::TriangleFan);
    sf::VertexArray cone(sf::TriangleFan);
    cone.append(sf::Vertex(shape.getPosition(), sf::Color(255, 255, 0, 150)));

    float fov = 50.0f;
    float viewDistance = 200.0f;
    int segments = 15;

    for (int i = 0; i <= segments; ++i) {
        float angle = enemyAngle - (fov / 2) + (i * (fov / segments));
        float rad = angle * (3.14159265358979323846 / 180.0f);
        sf::Vector2f endPoint = shape.getPosition() + sf::Vector2f(std::cos(rad), std::sin(rad)) * viewDistance;

        sf::Vector2f step = (endPoint - shape.getPosition()) / static_cast<float>(viewDistance / 5.0f);
        sf::Vector2f rayPos = shape.getPosition();
        for (float j = 0; j < viewDistance; j += 5.0f) {
            rayPos += step;
            int gridX = static_cast<int>(rayPos.x / CELL_SIZE);
            int gridY = static_cast<int>(rayPos.y / CELL_SIZE);
            if (!grid.getCell(gridX, gridY).walkable) {
                break;
            }
            endPoint = rayPos;
        }
        cone.append(sf::Vertex(endPoint, sf::Color(255, 255, 0, 150)));
    }
    window.draw(cone);
    /*cone.getBounds().intersects()*/
}
