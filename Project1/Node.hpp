#ifndef NODE_HPP
#define NODE_HPP

#include <SFML/System/Vector2.hpp>
#include <iostream>

class Node {
public:
    sf::Vector2i position;
    int gCost, hCost, fCost;
    Node* parent;

    Node(sf::Vector2i pos);
    void calculateCosts(sf::Vector2i endPos, int newG);
    int calculateHeuristic(sf::Vector2i endPos);
};

#endif