#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "EnemyPatroller.hpp"
#include "Enemy.hpp"
#include "Grid.hpp"
#include "EntityManager.hpp"
#include <vector>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 600;

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Jeu SFML - IA Ennemis");
    window.setFramerateLimit(60);

    Grid grid;

    EntityManager manager;
    auto player = std::make_shared<Player>(50, 50);
    manager.setPlayer(player);

    manager.addEnemy(std::make_shared<EnemyPatroller>(100, 100, sf::Vector2f(400, 200), sf::Vector2f(400, 500), sf::Vector2f(475, 500)));
    manager.addEnemy(std::make_shared<EnemyPatroller>(1000, 100, sf::Vector2f(1400, 200), sf::Vector2f(1400, 500), sf::Vector2f(1175, 500)));
    manager.addCamera(std::make_shared<CameraAI>(250, 50, &manager));
    manager.addCamera(std::make_shared<CameraAI>(50, 330, &manager));
    manager.addCamera(std::make_shared<CameraAI>(1030, 50, &manager));
    manager.addDogo(std::make_shared<ChasingDogo>(650, 200));

    // Ajouter un alli� au manager
    auto ally1 = std::make_shared<AllyAI>(200, 400, grid);
    manager.addAlly(ally1);

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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
        }

        /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (manager.getPlayer()) {
                manager.alertAllies(manager.getPlayer()->shape.getPosition());
            }
        }*/

        window.clear();

        grid.draw(window);
        manager.update(deltaTime, grid);
        manager.draw(window, grid);

        window.display();
    }
    return 0;
}
