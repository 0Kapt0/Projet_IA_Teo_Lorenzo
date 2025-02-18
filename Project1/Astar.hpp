#ifndef ASTAR_HPP
#define ASTAR_HPP

#include <vector>
#include <queue>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "Grid.hpp"

struct Node {
    sf::Vector2i position;
    float gCost, hCost, fCost;
    Node* parent;

    Node(sf::Vector2i pos, float g, float h, Node* p = nullptr)
        : position(pos), gCost(g), hCost(h), fCost(g + h), parent(p) {
    }

    bool operator<(const Node& other) const {
        return fCost > other.fCost;
    }
};

// Spécialisation de std::hash pour sf::Vector2i
namespace std {
    template <>
    struct hash<sf::Vector2i> {
        size_t operator()(const sf::Vector2i& v) const {
            return hash<int>()(v.x) ^ hash<int>()(v.y << 16);
        }
    };
}

std::vector<sf::Vector2i> astar_search(const sf::Vector2i& start, const sf::Vector2i& goal, const Grid& grid);

#endif // ASTAR_HPP
