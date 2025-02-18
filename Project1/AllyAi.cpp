#include "AllyAI.hpp"
#include <cmath>
#include <iostream>

using namespace sf;
using namespace std;

AllyAI::AllyAI(float x, float y, EntityManager* manager)
    : Entity(x, y, Color::Blue), entityManager(manager) {
    targetpos = Vector2f(x, y);
    lastKnownPosition = targetpos;
    reset();
    shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);
}

void AllyAI::update(float deltaTime, Grid& grid) {

}

void AllyAI::updateA(float deltaTime, Grid& grid, Player& player, vector<Enemy*>& nearbyEnemies) {
    this->deltaTime = deltaTime;
    shape.setRotation(allyAngle);

    float distanceToPlayer = sqrt(pow(player.shape.getPosition().x - shape.getPosition().x, 2) +
        pow(player.shape.getPosition().y - shape.getPosition().y, 2));

    // Rester proche du joueur
    if (distanceToPlayer > 100.0f) {
        targetpos = player.shape.getPosition();
        setAtTargetPosition(false);
    }

    GOAPPlannerAlly planner;
    GoalA currentGoal  = GoalA::Patrolling;

    for (auto& enemy : nearbyEnemies) {
        float distanceToEnemy = sqrt(pow(enemy->shape.getPosition().x - shape.getPosition().x, 2) +
            pow(enemy->shape.getPosition().y - shape.getPosition().y, 2));

        if (distanceToEnemy < 200.0f) {
            currentGoal = GoalA::Distract;
        }
    }

    vector<ActionAlly*> actions = planner.Plan(*this, currentGoal);

    for (ActionAlly* action : actions) {
        if (action->CanExecute(*this)) {
            action->Execute(*this);
            break;
        }
    }
}

//void EnemyDogo::computePathToPlayerA() {
//    if (!entityManager) return;
//
//    sf::Vector2i start((int)shape.getPosition().x / 32, (int)shape.getPosition().y / 32);
//    sf::Vector2i end((int)lastKnownPosition.x / 32, (int)lastKnownPosition.y / 32);
//
//    std::vector<sf::Vector2i> path = astar_search(start, end, entityManager->getGrid());
//
//    pathToPlayer = std::queue<sf::Vector2f>();
//    for (const auto& pos : path) {
//        pathToPlayer.push(sf::Vector2f(pos.x * 32, pos.y * 32));
//    }
//}

void AllyAI::reset() {
    // Reset ally state (Example implementation)
    cout << "Resetting AllyAI state.\n";
}

bool AllyAI::atTargetPosition() const {
    return (shape.getPosition() == targetpos);
}

void AllyAI::setAtTargetPosition(bool atTarget) {
    // This function should probably store the value in a member variable
    cout << "Setting atTargetPosition: " << atTarget << endl;
}

void AllyAI::rotateTowards(const sf::Vector2f& direction) {
    float dx = direction.x - shape.getPosition().x;
    float dy = direction.y - shape.getPosition().y;
    float angle = atan2(dy, dx) * 180 / 3.14159;
    shape.setRotation(angle);
}

void AllyAI::alertAllies(Vector2f targetpos) {
    cout << "Alerting allies!\n";
}


void AllyAI::throwDiversion(Vector2f pos) {
    cout << "Throwing diversion at (" << pos.x << ", " << pos.y << ")\n";
}

void AllyAI::openHidingSpot(Vector2f pos) {
    cout << "Opening hiding spot at (" << pos.x << ", " << pos.y << ")\n";
}

void AllyAI::blockEnemyPath(Vector2f pos) {
    cout << "Blocking enemy path at (" << pos.x << ", " << pos.y << ")\n";
}

bool ThrowDiversion::CanExecute(const AllyAI& state) {
    return true;
}

void ThrowDiversion::Execute(AllyAI& state) {
    state.throwDiversion(state.shape.getPosition() + Vector2f(50, 50));
}

bool OpenHidingSpot::CanExecute(const AllyAI& state) {
    return true;
}

void OpenHidingSpot::Execute(AllyAI& state) {
    state.openHidingSpot(state.shape.getPosition());
}

bool BlockEnemyPath::CanExecute(const AllyAI& state) {
    return true;
}

void BlockEnemyPath::Execute(AllyAI& state) {
    state.blockEnemyPath(state.shape.getPosition());
}

vector<ActionAlly*> GOAPPlannerAlly::Plan(const AllyAI& initialState, GoalA Goal) {
    vector<ActionAlly*> plan;

    if (Goal == GoalA::Distract) {
        plan.push_back(new ThrowDiversion());
    }
    else if (Goal == GoalA::Protect) {
        plan.push_back(new OpenHidingSpot());
    }
    else if (Goal == GoalA::Block) {
        plan.push_back(new BlockEnemyPath());
    }

    return plan;
}
