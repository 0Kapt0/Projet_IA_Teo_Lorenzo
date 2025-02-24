#include "CameraAI.hpp"
#include "EntityManager.hpp"
#include <cmath>

CameraAI::CameraAI(float x, float y, EntityManager* manager) : Enemy(x, y), entityManager(manager) {
    if (!cameraTexture.loadFromFile("assets/texture/CCTV.png")) {
        cerr << "Erreur chargement TEXTURE de la camera !" << endl;
    }

    shape.setTexture(&cameraTexture);
    shape.setScale(2.0f, 2);
    shape.setSize({ 40, 20 });
    shape.setFillColor(Color::White);
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2);
}

void CameraAI::update(float deltaTime, Grid& grid, Player& player) {
    static int direction = 1;
    static float targetAngle = 90.0f;

    if (cooldownClock.getElapsedTime().asSeconds() > cooldownTimeFreeze) {
        shape.rotate(direction * rotationSpeed * deltaTime);
    }
    if (cooldownClock.getElapsedTime().asSeconds() > cooldownTime) {
        direction *= -1;
        targetAngle += direction * 90.0f;
        cooldownClock.restart();
    }



    VertexArray cone = getViewConeShape(grid);
    FloatRect playerBounds = player.shape.getGlobalBounds();
    bool playerDetected = false;

    for (size_t i = 1; i < cone.getVertexCount() - 1; ++i) {
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

void CameraAI::draw(RenderWindow& window, Grid& grid) {
    VertexArray cone = getViewConeShape(grid);
    window.draw(cone);
    window.draw(shape);
}

bool CameraAI::isPlayerDetected(Player& player, Grid& grid) {
    VertexArray cone = getViewConeShape(grid);
    FloatRect playerBounds = player.shape.getGlobalBounds();

    for (size_t i = 1; i < cone.getVertexCount() - 1; ++i) {
        if (isTriangleIntersectingRect(cone[0].position, cone[i].position, cone[i + 1].position, playerBounds)) {
            return true;
        }
    }
    return false;
}

VertexArray CameraAI::getViewConeShape(Grid& grid) {
    VertexArray cone(TriangleFan);
    cone.append(Vertex(shape.getPosition(), Color(255, 255, 0, 150)));

    float fov = 50.0f;
    float viewDistance = 300.0f;
    int segments = 15;

    for (int i = 0; i <= segments; ++i) {
        float angle = shape.getRotation() - (fov / 2) + (i * (fov / segments));
        float rad = angle * (3.14159265358979323846 / 180.0f);
        Vector2f endPoint = shape.getPosition() + Vector2f(cos(rad), sin(rad)) * viewDistance;

        Vector2f step = (endPoint - shape.getPosition()) / static_cast<float>(viewDistance / 5.0f);
        Vector2f rayPos = shape.getPosition();
        for (float j = 0; j < viewDistance; j += 5.0f) {
            rayPos += step;
            int gridX = static_cast<int>(rayPos.x / CELL_SIZE);
            int gridY = static_cast<int>(rayPos.y / CELL_SIZE);
            if (!grid.getCell(gridX, gridY).walkable) {
                break;
            }
            endPoint = rayPos;
        }
        cone.append(Vertex(endPoint, Color(255, 255, 0, 150)));
    }
    return cone;
}

bool CameraAI::isTriangleIntersectingRect(Vector2f a, Vector2f b, Vector2f c, FloatRect rect) {
    if (rect.contains(a) || rect.contains(b) || rect.contains(c)) {
        return true;
    }

    Vector2f topLeft(rect.left, rect.top);
    Vector2f topRight(rect.left + rect.width, rect.top);
    Vector2f bottomLeft(rect.left, rect.top + rect.height);
    Vector2f bottomRight(rect.left + rect.width, rect.top + rect.height);

    return isPointInTriangle(topLeft, a, b, c) ||
        isPointInTriangle(topRight, a, b, c) ||
        isPointInTriangle(bottomLeft, a, b, c) ||
        isPointInTriangle(bottomRight, a, b, c);
}

bool CameraAI::isPointInTriangle(Vector2f p, Vector2f a, Vector2f b, Vector2f c) {
    float d1 = (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
    float d2 = (p.x - c.x) * (b.y - c.y) - (b.x - c.x) * (p.y - c.y);
    float d3 = (p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

void CameraAI::alertEnemies(Vector2f targetpos) {
    if (!entityManager) return;

   /* cout << "Camera Alert! Enemies notified!\n";*/
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