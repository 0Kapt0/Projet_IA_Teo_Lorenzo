#ifndef CHASING_DOGO_HPP
#define CHASING_DOGO_HPP

#include <SFML/Graphics.hpp>
#include <queue>
#include <vector>
#include <map>

class Grid;
class Player;

struct Vector2iComparator {
    bool operator()(const sf::Vector2i& a, const sf::Vector2i& b) const {
        return std::tie(a.x, a.y) < std::tie(b.x, b.y);
    }
};

class ChasingDogo {
public:
    ChasingDogo(float x, float y);
    void update(float deltaTime, Grid& grid, Player& player);
    void draw(sf::RenderWindow& window, Grid& grid);
    void computePathToPlayer(Grid& grid, const sf::Vector2f& playerPos);
    sf::RectangleShape getShape() const { return shape; }

private:
    sf::RectangleShape shape;
    std::queue<sf::Vector2f> pathToPlayer;
    float speed = 60.0f;
    std::vector<sf::Vector2i> debugPath;

    sf::Vector2f lastPosition;
    int stuckCounter = 0;
    int frameCounter = 0;  // 🔄 Permet de recalculer le chemin régulièrement
};

#endif // CHASING_DOGO_HPP
