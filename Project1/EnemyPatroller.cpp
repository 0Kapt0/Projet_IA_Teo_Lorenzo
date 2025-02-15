#include "EnemyPatroller.hpp"
#include <cmath>

using namespace sf;
using namespace std;

EnemyPatroller::EnemyPatroller(float x, float y) : Enemy(x, y) {
    shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);
}

void EnemyPatroller::update(float deltaTime, Grid& grid, Player& player) {
    enemyAngle += 1;
    shape.setRotation(enemyAngle);

    VertexArray cone = getViewConeShape(grid);
    FloatRect playerBounds = player.shape.getGlobalBounds();

    bool playerDetected = false;

    //Check if any triangle in the cone intersects with the player's bounding box
    for (size_t i = 1; i < cone.getVertexCount() - 1; ++i) {
        if (isTriangleIntersectingRect(cone[0].position, cone[i].position, cone[i + 1].position, playerBounds)) {
            playerDetected = true;
            break;
        }
    }

    if (playerDetected) {
        shape.setFillColor(Color::Green);
    }
    else {
        shape.setFillColor(Color::Red);
    }
}

bool EnemyPatroller::isTriangleIntersectingRect(Vector2f a, Vector2f b, Vector2f c, FloatRect rect) {
    //Check if any of the triangle�s vertices are inside the rectangle
    if (rect.contains(a) || rect.contains(b) || rect.contains(c)) {
        return true;
    }

    //Check if any of the rectangle�s corners are inside the triangle
    Vector2f topLeft(rect.left, rect.top);
    Vector2f topRight(rect.left + rect.width, rect.top);
    Vector2f bottomLeft(rect.left, rect.top + rect.height);
    Vector2f bottomRight(rect.left + rect.width, rect.top + rect.height);

    if (isPointInTriangle(topLeft, a, b, c) ||
        isPointInTriangle(topRight, a, b, c) ||
        isPointInTriangle(bottomLeft, a, b, c) ||
        isPointInTriangle(bottomRight, a, b, c)) {
        return true;
    }
    return false;
}

bool EnemyPatroller::isPointInTriangle(Vector2f p, Vector2f a, Vector2f b, Vector2f c) {
    float d1 = (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
    float d2 = (p.x - c.x) * (b.y - c.y) - (b.x - c.x) * (p.y - c.y);
    float d3 = (p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

VertexArray EnemyPatroller::getViewConeShape(Grid& grid) {
    VertexArray cone(TriangleFan);
    cone.append(Vertex(shape.getPosition(), Color(255, 255, 0, 150)));

    float fov = 50.0f;
    float viewDistance = 200.0f;
    int segments = 15;

    for (int i = 0; i <= segments; ++i) {
        float angle = enemyAngle - (fov / 2) + (i * (fov / segments));
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

void EnemyPatroller::drawViewCone(RenderWindow& window, Grid& grid) {
    VertexArray cone = getViewConeShape(grid);
    window.draw(cone);
}
