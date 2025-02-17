#include "EnemyDogo.hpp"
#include <cmath>
#include <iostream>

using namespace sf;
using namespace std;

EnemyDogo::EnemyDogo(float x, float y, EntityManager* manager) : Enemy(x, y), entityManager(manager) {
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

    // Détection par l'odeur
    if (distanceToPlayer <= 75.0f) {
        playerDetected = true;
        lastKnownPosition = player.shape.getPosition();
        setAtTargetPosition(false);
    }
    // Détection par le son
    if (player.getIsRunning() && distanceToPlayer <= 300.0f) {
        playerDetected = true;
        lastKnownPosition = player.shape.getPosition();
        setAtTargetPosition(false);
    }
    else {
        playerDetected = false;
    }

    GOAPPlannerD planner;
    Goal currentGoal = playerDetected ? Goal::Chasing : Goal::Patrolling;

    vector<ActionD*> actions = planner.Plan(*this, currentGoal);

    for (ActionD* action : actions) {
        if (action->CanExecute(*this)) {
            action->Execute(*this);
            break;
        }
    }

    // Vérifier si le Dogo touche le joueur
    if (shape.getGlobalBounds().intersects(player.shape.getGlobalBounds())) {
        //stunPlayer(player);
        alertEnemies(player.shape.getPosition());
    }
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

void EnemyDogo::reset() {
    playerDetected = false;
    atTarget = false;
}

bool EnemyDogo::atTargetPosition() const {
    return atTarget;
}

void EnemyDogo::setAtTargetPosition(bool value) {
    atTarget = value;
}

void EnemyDogo::rotateTowards(const Vector2f& direction) {
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

bool ChasePlayerD::CanExecute(const EnemyDogo& state) {
    return state.playerDetected || !state.atTargetPosition();
}

void ChasePlayerD::Execute(EnemyDogo& state) {
    cout << "Dogo moving to last known position of the player\n";
    Vector2f direction = state.lastKnownPosition - state.shape.getPosition();
    float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (magnitude > 5.0f) {
        direction /= magnitude;
        state.rotateTowards(direction);
        state.shape.move(direction * state.SPEED * state.deltaTime);
    }
    else {
        state.setAtTargetPosition(true);
    }
}

vector<ActionD*> GOAPPlannerD::Plan(const EnemyDogo& initialState, Goal goal) {
    vector<ActionD*> plan;

    if (goal == Goal::Chasing) {
        plan.push_back(new ChasePlayerD());
    }
    else if (goal == Goal::Patrolling) {
        plan.push_back(new ChasePlayerD());
    }

    return plan;
}
