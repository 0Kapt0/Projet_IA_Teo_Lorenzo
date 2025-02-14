#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "Enemy.hpp"
#include "EnemyPatroller.hpp"
#include "Grid.hpp"
#include <vector>


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Jeu SFML - IA Ennemis");
    window.setFramerateLimit(60);

    Player player(200, 400);
    std::vector<EnemyPatroller> enemies = { EnemyPatroller(100, 100), EnemyPatroller(700, 100) };
    Grid grid;
    grid.loadFromFile("map.txt");

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Time dt = clock.restart();
        float deltaTime = dt.asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        player.update(deltaTime, grid);
        for (auto& enemy : enemies) {
            enemy.update(deltaTime, grid, player);
        }

        window.clear();
        grid.draw(window);
        window.draw(player.shape);
        for (auto& enemy : enemies) 
        {
            window.draw(enemy.shape);
            enemy.drawViewCone(window, grid);
        }
        window.display();
    }
    return 0;
}

