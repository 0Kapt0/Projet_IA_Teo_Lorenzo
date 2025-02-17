#include "CameraAI.hpp"
#include "EntityManager.hpp"
#include <cmath>

CameraAI::CameraAI(float x, float y, EntityManager* manager) : Enemy(x, y), entityManager(manager) {
    shape.setSize({ 40, 20 });
    shape.setFillColor(sf::Color::Green);
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2);
}

void CameraAI::update(float deltaTime, Grid& grid, Player& player) {
    static int direction = 1;
    static float targetAngle = 90.0f;

    if (std::abs(shape.getRotation() - targetAngle) < 1.0f) {
        direction *= -1;
        targetAngle += direction * 90.0f;
    }
    shape.rotate(direction * rotationSpeed * deltaTime);

    sf::VertexArray cone = getViewConeShape(grid);
    sf::FloatRect playerBounds = player.shape.getGlobalBounds();
    bool playerDetected = false;

    for (std::size_t i = 1; i < cone.getVertexCount() - 1; ++i) {
        if (isTriangleIntersectingRect(cone[0].position, cone[i].position, cone[i + 1].position, playerBounds)) {
            state = CameraState::ALERT;
            alertTimer = 0.0f;
            alertEnemies(player.shape.getPosition());
            playerDetected = true;
            break;
        }
        else {
            desalertEnemies(player.shape.getPosition());
        }

    }
}

void CameraAI::draw(sf::RenderWindow& window, Grid& grid) {
    sf::VertexArray cone = getViewConeShape(grid);
    window.draw(cone);
    window.draw(shape);
}

bool CameraAI::isPlayerDetected(Player& player, Grid& grid) {
    sf::VertexArray cone = getViewConeShape(grid);
    sf::FloatRect playerBounds = player.shape.getGlobalBounds();

    for (std::size_t i = 1; i < cone.getVertexCount() - 1; ++i) {
        if (isTriangleIntersectingRect(cone[0].position, cone[i].position, cone[i + 1].position, playerBounds)) {
            return true;
        }
    }
    return false;
}

sf::VertexArray CameraAI::getViewConeShape(Grid& grid) {
    sf::VertexArray cone(sf::TriangleFan);
    cone.append(sf::Vertex(shape.getPosition(), sf::Color(255, 255, 0, 150)));

    float fov = 50.0f;
    float viewDistance = 300.0f;
    int segments = 15;

    for (int i = 0; i <= segments; ++i) {
        float angle = shape.getRotation() - (fov / 2) + (i * (fov / segments));
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
    return cone;
}

bool CameraAI::isTriangleIntersectingRect(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::FloatRect rect) {
    if (rect.contains(a) || rect.contains(b) || rect.contains(c)) {
        return true;
    }

    sf::Vector2f topLeft(rect.left, rect.top);
    sf::Vector2f topRight(rect.left + rect.width, rect.top);
    sf::Vector2f bottomLeft(rect.left, rect.top + rect.height);
    sf::Vector2f bottomRight(rect.left + rect.width, rect.top + rect.height);

    return isPointInTriangle(topLeft, a, b, c) ||
        isPointInTriangle(topRight, a, b, c) ||
        isPointInTriangle(bottomLeft, a, b, c) ||
        isPointInTriangle(bottomRight, a, b, c);
}

bool CameraAI::isPointInTriangle(sf::Vector2f p, sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) {
    float d1 = (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
    float d2 = (p.x - c.x) * (b.y - c.y) - (b.x - c.x) * (p.y - c.y);
    float d3 = (p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

void CameraAI::alertEnemies(Vector2f targetpos) {
    if (!entityManager) return;

    std::cout << "Camera Alert! Enemies notified!\n";
    for (auto& enemy : entityManager->getEnemies()) {
        enemy->setWarning(true, (targetpos));
    }
}

void CameraAI::desalertEnemies(Vector2f targetpos) {
    if (!entityManager) return;

    for (auto& enemy : entityManager->getEnemies()) {
        enemy->setWarning(false, targetpos);
    }
}