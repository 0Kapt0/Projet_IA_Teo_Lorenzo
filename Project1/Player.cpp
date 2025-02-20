// player.cpp
#include "Player.hpp"

#include <SFML/Window/Keyboard.hpp>

Player::Player(float x, float y) : Entity(x, y, Color::Blue) {
    /*shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);*/
}

void Player::update(float deltaTime, Grid& grid) {
    Vector2f movement(0.f, 0.f);
    if (Keyboard::isKeyPressed(Keyboard::Z)) movement.y -= SPEED * deltaTime;
    if (Keyboard::isKeyPressed(Keyboard::S)) movement.y += SPEED * deltaTime;
    if (Keyboard::isKeyPressed(Keyboard::Q)) movement.x -= SPEED * deltaTime;
    if (Keyboard::isKeyPressed(Keyboard::D)) movement.x += SPEED * deltaTime;

    if (Keyboard::isKeyPressed(Keyboard::LShift)) {
        SPEED = 300.f;
        isRunning = true;
    }
    else {
        SPEED = 200.f;
        isRunning = false;
    }
    if (Keyboard::isKeyPressed(Keyboard::Space)) {

        cookies.push_back(make_unique<Cookie>(shape.getPosition().x, shape.getPosition().y));
    }
    Vector2f newPosition = shape.getPosition() + movement;
    FloatRect newBounds(newPosition, shape.getSize());

    // Vérifier les quatre coins du joueur
    auto isWalkable = [&](float x, float y) {
        int gridX = static_cast<int>(x / CELL_SIZE);
        int gridY = static_cast<int>(y / CELL_SIZE);
        return gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT && grid.getCell(gridX, gridY).walkable;
        };

    if (isWalkable(newBounds.left, newBounds.top) &&
        isWalkable(newBounds.left + newBounds.width - 1, newBounds.top) &&
        isWalkable(newBounds.left, newBounds.top + newBounds.height - 1) &&
        isWalkable(newBounds.left + newBounds.width - 1, newBounds.top + newBounds.height - 1)) {
        shape.move(movement);
    }
}

void Player::cookiedraw(RenderWindow& window){
    for (auto& cookie : cookies)
    {
        window.draw(cookie->shape);
    }
}

bool Player::getIsRunning() {
    return isRunning;
}