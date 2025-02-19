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
}

void ChasingDogo::update(float deltaTime, Grid& grid, Player& player) {
    // 🔄 Recalcule le chemin toutes les 30 frames
    if (frameCounter % 30 == 0) {
        computePathToPlayer(grid, player.getPosition());
    }
    frameCounter++;

    if (pathToPlayer.empty()) {
        cout << "🔄 Aucun chemin valide, recalcul immédiat..." << endl;
        computePathToPlayer(grid, player.getPosition());
        return;
    }

    Vector2f nextPos = pathToPlayer.front();
    Vector2f direction = nextPos - shape.getPosition();
    float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);

    // ✅ Vérification si la prochaine case est un mur
    Vector2i nextGridPos(
        static_cast<int>(nextPos.x / CELL_SIZE),
        static_cast<int>(nextPos.y / CELL_SIZE)
    );

    if (!grid.isWalkable(nextGridPos.x, nextGridPos.y)) {
        cout << "🚨 Prochain mouvement bloqué en (" << nextGridPos.x << "," << nextGridPos.y << "), recalcul..." << endl;
        computePathToPlayer(grid, player.getPosition());
        return;
    }

    // 🛠 Ajustement pour éviter les collisions avec les murs
    float margin = 2.0f;  // Petite marge pour éviter de coller aux murs
    Vector2f futurePos = shape.getPosition() + (direction * speed * deltaTime);

    Vector2i futureGridPos(
        static_cast<int>((futurePos.x + margin) / CELL_SIZE),
        static_cast<int>((futurePos.y + margin) / CELL_SIZE)
    );

    if (!grid.isWalkable(futureGridPos.x, futureGridPos.y)) {
        cout << "🚨 Le Dogo va entrer en collision avec un mur ! Recalcul..." << endl;
        computePathToPlayer(grid, player.getPosition());
        return;
    }

    // ✅ Déplacement du Dogo
    if (magnitude > 2.0f) {
        direction /= magnitude;

        // ✅ Vérifier si le Dogo est bloqué en tentant d'aller à droite
        Vector2f futurePos = shape.getPosition() + (direction * speed * deltaTime);
        Vector2i futureGridPos(
            static_cast<int>(futurePos.x / CELL_SIZE),
            static_cast<int>(futurePos.y / CELL_SIZE)
        );

        if (!grid.isWalkable(futureGridPos.x, futureGridPos.y)) {
            cout << "🚧 Blocage détecté en (" << futureGridPos.x << "," << futureGridPos.y << "), recalcul..." << endl;
            computePathToPlayer(grid, player.getPosition());
        }
        else {
            shape.move(direction * speed * deltaTime);
        }
    }


    // 🔄 Vérification si le Dogo est bloqué depuis longtemps
    if (shape.getPosition() == lastPosition) {
        stuckCounter++;
        if (stuckCounter > 20) {  // Augmenté pour éviter les recalculs inutiles
            cout << "🚨 Dogo semble coincé ! Recalcul forcé..." << endl;
            computePathToPlayer(grid, player.getPosition());
            stuckCounter = 0;
        }
    }
    else {
        stuckCounter = 0;
    }

    lastPosition = shape.getPosition();
}




void ChasingDogo::draw(RenderWindow& window, Grid& grid) {
    window.draw(shape);

    // 🔥 Dessine le chemin suivi par le Dogo en JAUNE
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
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
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
                pathToPlayer.push(Vector2f(pos.x * CELL_SIZE, pos.y * CELL_SIZE));
                if (pos.x >= 0 && pos.x < GRID_WIDTH && pos.y >= 0 && pos.y < GRID_HEIGHT) {
                    grid.debugPath.push_back(pos);
                }
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


