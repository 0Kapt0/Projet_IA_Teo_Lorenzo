#include <SFML/Graphics.hpp>
#include "Player.hpp"
#include "EnemyPatroller.hpp"
#include "Enemy.hpp"
#include "Grid.hpp"
#include "EntityManager.hpp"
#include <vector>


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main() {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Jeu SFML - IA Ennemis");
    window.setFramerateLimit(60);

    Grid grid;

    EntityManager manager;
    auto player = make_shared<Player>(200, 400);
    manager.setPlayer(player);
    manager.addEnemy(make_shared<EnemyPatroller>(100, 100));
    //manager.addEnemy(make_shared<EnemyPatroller>(700, 100));
    manager.addCamera(std::make_shared<CameraAI>(250, 50, &manager));
    grid.loadFromFile("map.txt");

    Clock clock;

    while (window.isOpen()) {
        Time dt = clock.restart();
        float deltaTime = dt.asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }
        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        window.clear();


        grid.draw(window);
        manager.update(deltaTime, grid);
        manager.draw(window, grid);

        window.display();
    }
    return 0;
}

