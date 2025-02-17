#include "EnemyDogo.hpp"
#include <cmath>
#include <iostream>

using namespace sf;
using namespace std;

EnemyDogo::EnemyDogo(float x, float y, EntityManager* manager) : Enemy(x, y), entityManager(manager) {
    targetpos = Vector2f(x, y);
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
        targetpos = player.shape.getPosition();
        setAtTargetPosition(false);
    }
    // Détection par le son
    else if (/*player.isRunning() &&*/ distanceToPlayer <= 300.0f) {
        playerDetected = true;
        targetpos = player.shape.getPosition();
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

//void EnemyDogo::stunPlayer(Player& player) {
//    player.setStunned(true);
//    cout << "Dogo stunned the player!\n";
//}

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
    return state.playerDetected && !state.atTargetPosition();
}

void ChasePlayerD::Execute(EnemyDogo& state) {
    cout << "Dogo chasing player\n";
    state.targetpos = state.lastKnownPosition;
    state.setAtTargetPosition(false);
}

vector<ActionD*> GOAPPlannerD::Plan(const EnemyDogo& initialState, Goal goal) {
    vector<ActionD*> plan;

    if (goal == Goal::Chasing) {
        plan.push_back(new ChasePlayerD());
    }
    else if (goal == Goal::Patrolling) {
    }

    return plan;
}
