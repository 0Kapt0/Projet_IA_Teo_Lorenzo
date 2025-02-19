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
    /*shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);*/
    alignToGrid();
}

void ChasingDogo::update(float deltaTime, Grid& grid, Player& player) {
    frameCounter++;

    // 🔄 Recalcule le chemin toutes les 30 frames ou si la file est vide
    if (frameCounter % 30 == 0 || pathToPlayer.empty()) {
        computePathToPlayer(grid, player.getPosition());
    }

    if (pathToPlayer.empty()) return;

    Vector2f currentPos = shape.getPosition();
    Vector2f targetPos = pathToPlayer.front();
    Vector2f direction = targetPos - currentPos;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    // ✅ Détecter si on est trop proche du prochain point et passer au suivant
    if (distance < 5.0f) {
        pathToPlayer.pop();
        if (pathToPlayer.empty()) return; // Fin du chemin
        targetPos = pathToPlayer.front();
        direction = targetPos - currentPos;
        distance = sqrt(direction.x * direction.x + direction.y * direction.y);
    }

    // ✅ Correction des angles bloqués
    Vector2f nextPos = currentPos + (direction / distance) * speed * deltaTime;

    // Vérifier si on approche un angle et ajuster
    Vector2i gridPos = Vector2i(static_cast<int>(nextPos.x / CELL_SIZE), static_cast<int>(nextPos.y / CELL_SIZE));
    Vector2i futureGridPos = Vector2i(static_cast<int>((nextPos.x + direction.x * speed * deltaTime) / CELL_SIZE),
                                      static_cast<int>((nextPos.y + direction.y * speed * deltaTime) / CELL_SIZE));

    if (!grid.isWalkable(futureGridPos.x, futureGridPos.y)) {
        // 🔄 Essayer d'ajuster en priorisant l'axe avec le plus grand mouvement
        if (abs(direction.x) > abs(direction.y)) {
            nextPos.x = currentPos.x + (direction.x / abs(direction.x)) * speed * deltaTime;
        } else {
            nextPos.y = currentPos.y + (direction.y / abs(direction.y)) * speed * deltaTime;
        }
    }

    // ✅ Déplacement final sans blocage
    shape.setPosition(nextPos);
}






void ChasingDogo::alignToGrid() {
    Vector2f pos = shape.getPosition();
    int gridX = static_cast<int>(pos.x / CELL_SIZE);
    int gridY = static_cast<int>(pos.y / CELL_SIZE);

    shape.setPosition(
        (gridX * CELL_SIZE) + (CELL_SIZE / 2),
        (gridY * CELL_SIZE) + (CELL_SIZE / 2)
    );
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

    // 🔥 Ajout des directions, mais filtrage des diagonales dangereuses
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

            // 🔴 Vérifie si on est en diagonale
            bool isDiagonal = abs(dir.x) == 1 && abs(dir.y) == 1;

            if (isDiagonal) {
                // 🔴 Vérification pour empêcher de traverser un coin de mur en diagonale
                Vector2i check1 = { current.pos.x + dir.x, current.pos.y };
                Vector2i check2 = { current.pos.x, current.pos.y + dir.y };

                if (!grid.isWalkable(check1.x, check1.y) || !grid.isWalkable(check2.x, check2.y)) {
                    continue; // 🚧 Ignore la diagonale bloquée
                }
            }

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




