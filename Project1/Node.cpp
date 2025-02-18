#include "Node.hpp"

Node::Node(sf::Vector2i pos)
    : position(pos), gCost(0), hCost(0), fCost(0), parent(nullptr) {
}

void Node::calculateCosts(sf::Vector2i endPos, int newG) {
    gCost = newG;
    hCost = calculateHeuristic(endPos);
    fCost = gCost + hCost;
}

int Node::calculateHeuristic(sf::Vector2i endPos) {
    int dx = std::abs(endPos.x - position.x);
    int dy = std::abs(endPos.y - position.y);

    // Distance diagonale (Octile Distance) ? Meilleure que l'Euclidienne
    return 10 * (dx + dy) + (14 - 2 * 10) * std::min(dx, dy);
}