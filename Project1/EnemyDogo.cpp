#include "EnemyDogo.hpp"
#include <cmath>
#include <iostream>

using namespace sf;
using namespace std;

EnemyDogo::EnemyDogo(float x, float y, EntityManager* manager) : Enemy(x, y), entityManager(manager) {
    if (!dogoTexture.loadFromFile("assets/texture/DOGO.png")) {
        cerr << "Erreur chargement TEXTURE de la camera !" << endl;
    }

    shape.setTexture(&dogoTexture);
    shape.setScale(2.0f, 2.0f);
    targetpos = Vector2f(x, y);
    lastKnownPosition = targetpos;
    reset();
    shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);
}

void EnemyDogo::update(float deltaTime, Grid& grid, Player& player, vector<Enemy*>& nearbyEnemies) {
    this->deltaTime = deltaTime;
    shape.setRotation(enemyAngle);

    float distanceToPlayer = sqrt(pow(player.shape.getPosition().x - shape.getPosition().x, 2) +
        pow(player.shape.getPosition().y - shape.getPosition().y, 2));

    // 📌 Détection par l'odeur
    if (distanceToPlayer <= 75.0f) {
        playerDetected = true;
        lastKnownPosition = player.shape.getPosition();
        setAtTargetPosition(false);
    }
    // 📌 Détection par le son
    if (player.getIsRunning() && distanceToPlayer <= 300.0f) {
        playerDetected = true;
        lastKnownPosition = player.shape.getPosition();
        setAtTargetPosition(false);
    }
    else {
        playerDetected = false;
    }

    // 📌 Le Dogo NE DOIT PAS suivre le joueur directement, juste utiliser A*
    GOAPPlannerD planner;
    Goal currentGoal = playerDetected ? Goal::Chasing : Goal::Patrolling;

    vector<ActionD*> actions = planner.Plan(*this, currentGoal);
    for (ActionD* action : actions) {
        if (action->CanExecute(*this)) {
            action->Execute(*this);
            break;
        }
    }

    // 📌 Vérifier si le Dogo touche le joueur
    if (shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
        alertEnemies(player.shape.getPosition());
    }
}

void EnemyDogo::computePathToPlayer() {
    if (!entityManager) return;

    sf::Vector2i start(
        static_cast<int>(shape.getPosition().x / CELL_SIZE),
        static_cast<int>(shape.getPosition().y / CELL_SIZE)
    );

    sf::Vector2i end(
        static_cast<int>(lastKnownPosition.x / CELL_SIZE),
        static_cast<int>(lastKnownPosition.y / CELL_SIZE)
    );

    Grid& grid = entityManager->getGrid();
    std::queue<sf::Vector2i> openSet;
    std::map<sf::Vector2i, sf::Vector2i, Vector2iComparator> cameFrom;
    std::vector<sf::Vector2i> path;

    openSet.push(start);
    cameFrom[start] = start;

    // 📌 Directions (haut, bas, gauche, droite, et diagonales)
    std::vector<sf::Vector2i> directions = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},  // Haut, Bas, Gauche, Droite
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1} // Diagonales
    };

    while (!openSet.empty()) {
        sf::Vector2i current = openSet.front();
        openSet.pop();

        // 📌 Si on a trouvé la cible, on reconstruit le chemin
        if (current == end) {
            while (current != start) {
                path.push_back(current);
                current = cameFrom[current];
            }
            std::reverse(path.begin(), path.end());

            // 📌 Convertir le chemin en file d'attente utilisable par le Dogo
            pathToPlayer = std::queue<sf::Vector2f>();
            for (const auto& pos : path) {
                pathToPlayer.push(sf::Vector2f(pos.x * CELL_SIZE, pos.y * CELL_SIZE));
            }
            return;
        }

        // 📌 Explorer les voisins
        for (const auto& dir : directions) {
            sf::Vector2i neighbor = current + dir;

            if (grid.isWalkable(neighbor.x, neighbor.y) && cameFrom.find(neighbor) == cameFrom.end()) {
                openSet.push(neighbor);
                cameFrom[neighbor] = current;
            }
        }
    }

    // 📌 Aucun chemin trouvé : vider le path
    pathToPlayer = std::queue<sf::Vector2f>();
}



bool ChasePlayerD::CanExecute(const EnemyDogo& state) {
    return state.playerDetected || !state.atTargetPosition();
}

void ChasePlayerD::Execute(EnemyDogo& state) {
    cout << "Dogo moving along BFS path\n";

    if (state.pathToPlayer.empty()) {
        state.computePathToPlayer(); // Recalculer un chemin s'il n'y en a pas
    }

    if (!state.pathToPlayer.empty()) {
        sf::Vector2f nextPos = state.pathToPlayer.front();
        sf::Vector2f direction = nextPos - state.shape.getPosition();
        float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);

        if (magnitude < 5.0f) {
            state.pathToPlayer.pop();
            return;
        }

        direction /= magnitude;
        state.rotateTowards(direction);

        // 📌 Vérifier la future position pour éviter les murs
        sf::Vector2f newPos = state.shape.getPosition() + (direction * state.SPEED * state.deltaTime);
        sf::FloatRect futureBounds = state.shape.getGlobalBounds();
        futureBounds.left = newPos.x;
        futureBounds.top = newPos.y;

        Grid& grid = state.entityManager->getGrid();
        sf::Vector2i checkPos(static_cast<int>(futureBounds.left / CELL_SIZE), static_cast<int>(futureBounds.top / CELL_SIZE));

        if (grid.isWalkable(checkPos.x, checkPos.y)) {
            state.shape.setPosition(newPos);
        }
        else {
            cout << "🚧 Dogo bloqué, recalcul du chemin\n";
            state.computePathToPlayer();
        }
    }
}



vector<ActionD*> GOAPPlannerD::Plan(const EnemyDogo& initialState, Goal goal) {
    vector<ActionD*> plan;

    if (goal == Goal::Chasing) {
        plan.push_back(new ChasePlayerD());
    }
    else if (goal == Goal::Patrolling) {
        plan.push_back(new ChasePlayerD());  // Ajoute d'autres actions ici si nécessaire
    }

    return plan;
}

void EnemyDogo::rotateTowards(const sf::Vector2f& direction) {
    if (direction.x != 0 || direction.y != 0) {
        float targetAngle = atan2(direction.y, direction.x) * (180.0f / 3.14159265358979323846f);
        float angleDifference = targetAngle - enemyAngle;

        if (angleDifference > 180.0f) angleDifference -= 360.0f;
        if (angleDifference < -180.0f) angleDifference += 360.0f;

        float rotationStep = maxRotationSpeed * deltaTime;
        if (fabs(angleDifference) <= rotationStep) {
            enemyAngle = targetAngle;
        }
        else {
            enemyAngle += (angleDifference > 0 ? 1 : -1) * rotationStep;
        }
    }
}

void EnemyDogo::setAtTargetPosition(bool value) {
    atTarget = value;
}

bool EnemyDogo::atTargetPosition() const {
    return atTarget;
}

void EnemyDogo::reset() {
    playerDetected = false;
    atTarget = false;
}

void EnemyDogo::alertEnemies(Vector2f targetpos) {
    if (!entityManager) return;

    cout << "Dogo Alert! Enemies notified!\n";
    for (auto& enemy : entityManager->getEnemies()) {
        enemy->setWarning(true, targetpos);
    }
}

void EnemyDogo::desalertEnemies(Vector2f targetpos) {
    if (!entityManager) return;

    for (auto& enemy : entityManager->getEnemies()) {
        enemy->setWarning(false, targetpos);
    }
}