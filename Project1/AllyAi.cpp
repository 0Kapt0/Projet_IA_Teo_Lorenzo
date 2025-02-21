#include "AllyAI.hpp"
#include <cmath>
#include <queue>

using namespace sf;
using namespace std;

AllyAI::AllyAI(float x, float y, Grid& grid) : targetPos(x, y), grid(grid) {
    if (!allyTexture.loadFromFile("assets/texture/ALLY.png")) {
        cerr << "Erreur chargement TEXTURE de la camera !" << endl;
    }

    shape.setTexture(&allyTexture);
    shape.setSize(Vector2f(50, 50));
    shape.setOrigin(shape.getSize().x / 2, shape.getSize().y / 2);
    shape.setPosition(x, y);

    // Création du Behavior Tree
    auto root = make_unique<SelectorNode>();

    auto sequence = make_unique<SequenceNode>();
    sequence->addChild(make_unique<ConditionNode>([this]() {
        return (shape.getPosition() != targetPos);
        }));

    sequence->addChild(make_unique<ActionNode>([this]() {
        this->computePathToTarget(this->targetPos);
        return NodeState::RUNNING;
        }));

    root->addChild(std::move(sequence));
    root->addChild(make_unique<ActionNode>([]() {
        return NodeState::SUCCESS;
        }));

    behaviorTree = std::move(root);
}

void AllyAI::update(float deltaTime) {
    behaviorTree->execute();
    moveTowardsTarget(deltaTime);
}

void AllyAI::computePathToTarget(const Vector2f& targetPos) {
    Vector2i start(
        static_cast<int>(shape.getPosition().x / CELL_SIZE),
        static_cast<int>(shape.getPosition().y / CELL_SIZE)
    );
    Vector2i end(
        static_cast<int>(targetPos.x / CELL_SIZE),
        static_cast<int>(targetPos.y / CELL_SIZE)
    );

    if (!grid.isWalkable(end.x, end.y)) {
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

            pathToTarget = queue<Vector2f>();
            for (const auto& pos : path) {
                Vector2f alignedPos(
                    (pos.x * CELL_SIZE) + (CELL_SIZE / 2),
                    (pos.y * CELL_SIZE) + (CELL_SIZE / 2)
                );
                pathToTarget.push(alignedPos);
            }
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


void AllyAI::alert(Vector2f newTarget) {
    std::cout << "AllyAI alerted to position: (" << newTarget.x << ", " << newTarget.y << ")\n";
    targetPos = newTarget;
}

void AllyAI::moveTowardsTarget(float deltaTime) {
    if (pathToTarget.empty()) return;

    Vector2f currentPos = shape.getPosition();
    Vector2f target = pathToTarget.front();

    Vector2f direction = target - currentPos;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance < 5.0f) {
        pathToTarget.pop();
        return;
    }

    direction /= distance;
    shape.move(direction * 100.0f * deltaTime);
}

void AllyAI::draw(RenderWindow& window) {
    window.draw(shape);
}
