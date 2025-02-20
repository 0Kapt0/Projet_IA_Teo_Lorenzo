#include "EnemyDogo.hpp"
#include "Grid.hpp"
#include "Player.hpp"
#include <iostream>
#include <queue>
#include <cmath>

using namespace sf;
using namespace std;

ChasingDogo::ChasingDogo(float x, float y) {
    shape.setSize(Vector2f(30.0f, 30.0f));
    shape.setFillColor(Color::Red);
    shape.setPosition(x, y);
    lastPosition = shape.getPosition();
    shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);
}

void ChasingDogo::update(float deltaTime, Grid& grid, Player& player, EntityManager& entityManager) {
    if (!player.getIsRunning()) {
        return;
    }

    frameCounter++;

    if (frameCounter % 30 == 0 || pathToPlayer.empty()) {
        computePathToPlayer(grid, player.getPosition());
    }

    if (!pathToPlayer.empty()) {
        moveTowardsTarget(deltaTime);
    }

    checkCollisionWithPlayer(player, entityManager);
}

void ChasingDogo::moveTowardsTarget(float deltaTime) {
    Vector2f currentPos = shape.getPosition();
    Vector2f targetPos = pathToPlayer.front();

    Vector2f direction = targetPos - currentPos;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0) {
        direction /= distance;
    }

    float moveStep = speed * deltaTime;

    if (distance < moveStep) {
        shape.setPosition(targetPos);
        pathToPlayer.pop();
        return;
    }

    if (!pathToPlayer.empty()) {
        moveStep = adjustSpeedForTurn(currentPos, moveStep);
    }

    shape.move(direction * moveStep);
}

float ChasingDogo::adjustSpeedForTurn(Vector2f currentPos, float moveStep) {
    Vector2f nextPos = pathToPlayer.front();
    bool isTurning = (abs(nextPos.x - currentPos.x) > 0 && abs(nextPos.y - currentPos.y) > 0);

    if (isTurning) {
        return moveStep * 0.8f;
    }

    return moveStep;
}

void ChasingDogo::checkCollisionWithPlayer(Player& player, EntityManager& entityManager) {
    if (shape.getGlobalBounds().intersects(player.getShape().getGlobalBounds())) {
        for (auto& enemy : entityManager.getEnemies()) {
            enemy->setWarning(true, player.getPosition());
        }
    }
}

void ChasingDogo::draw(RenderWindow& window, Grid& grid) {
    window.draw(shape);

    for (auto& pos : debugPath) {
        if (pos.x >= 0 && pos.x < GRID_WIDTH && pos.y >= 0 && pos.y < GRID_HEIGHT) {
            RectangleShape debugCell(Vector2f(CELL_SIZE, CELL_SIZE));
            debugCell.setPosition(pos.x * CELL_SIZE, pos.y * CELL_SIZE);
            debugCell.setFillColor(Color::Yellow);
            debugCell.setOutlineThickness(1);
            debugCell.setOutlineColor(Color::Black);
            window.draw(debugCell);
        }
    }
}

void ChasingDogo::computePathToPlayer(Grid& grid, const Vector2f& playerPos) {
    Vector2i start(
        static_cast<int>(shape.getPosition().x / CELL_SIZE),
        static_cast<int>(shape.getPosition().y / CELL_SIZE)
    );
    Vector2i end(
        static_cast<int>(playerPos.x / CELL_SIZE),
        static_cast<int>(playerPos.y / CELL_SIZE)
    );

    if (!grid.isWalkable(end.x, end.y)) {
        cout << "🚨 Destination bloquée, recalcul impossible !" << endl;
        return;
    }

    struct Node {
        Vector2i pos;
        float cost;
        float heuristic;
        bool operator>(const Node& other) const {
            return (cost + heuristic) > (other.cost + other.heuristic);
        }
    };

    priority_queue<Node, vector<Node>, greater<Node>> openSet;
    map<Vector2i, Vector2i, Vector2iComparator> cameFrom;
    map<Vector2i, float, Vector2iComparator> costSoFar;
    vector<Vector2i> path;

    openSet.push({ start, 0, static_cast<float>(abs(start.x - end.x) + abs(start.y - end.y)) });
    cameFrom[start] = start;
    costSoFar[start] = 0;

    vector<Vector2i> directions = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},   // Droite, Gauche, Haut, Bas
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1}  // Diagonales
    };

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current.pos == end) {
            while (current.pos != start) {
                path.push_back(current.pos);
                current.pos = cameFrom[current.pos];
            }
            reverse(path.begin(), path.end());

            pathToPlayer = queue<Vector2f>();
            grid.debugPath.clear();

            cout << "🟡 Nouveau chemin : ";
            for (const auto& pos : path) {
                Vector2f alignedPos(
                    (pos.x * CELL_SIZE) + (CELL_SIZE / 2),
                    (pos.y * CELL_SIZE) + (CELL_SIZE / 2)
                );

                pathToPlayer.push(alignedPos);
                grid.debugPath.push_back(pos);
                cout << "(" << pos.x << "," << pos.y << ") ";
            }
            cout << endl;
            return;
        }

        for (const auto& dir : directions) {
            Vector2i neighbor = current.pos + dir;

            if (!grid.isWalkable(neighbor.x, neighbor.y)) continue;

            float newCost = costSoFar[current.pos] + 1;
            if (costSoFar.find(neighbor) == costSoFar.end() || newCost < costSoFar[neighbor]) {
                costSoFar[neighbor] = newCost;
                float priority = newCost + static_cast<float>(abs(neighbor.x - end.x) + abs(neighbor.y - end.y));
                openSet.push({ neighbor, newCost, priority });
                cameFrom[neighbor] = current.pos;
            }
        }
    }
}
