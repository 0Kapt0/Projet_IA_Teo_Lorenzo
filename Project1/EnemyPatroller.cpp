#include "EnemyPatroller.hpp"
#include <cmath>

using namespace sf;
using namespace std;

EnemyPatroller::EnemyPatroller(float x, float y) : Enemy(x, y) {
    targetpos = Vector2f(x, y);
    reset();
    shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);
}

void EnemyPatroller::update(float deltaTime, Grid& grid, Player& player) {
    this->deltaTime = deltaTime;
    shape.setRotation(enemyAngle);

    VertexArray cone = getViewConeShape(grid);
    FloatRect playerBounds = player.shape.getGlobalBounds();
    playerDetected = false;

    for (size_t i = 1; i < cone.getVertexCount() - 1; ++i) {
        if (isTriangleIntersectingRect(cone[0].position, cone[i].position, cone[i + 1].position, playerBounds)) {
            warning = true;
            playerDetected = true;
            targetpos = player.shape.getPosition();
            setAtTargetPosition(false);
            break;
        }
    }
    GOAPPlanner planner;
    Goal currentGoal;

    if (playerDetected) {
        currentGoal = Goal::Chasing;
    }
    else if (warning) {
        currentGoal = Goal::LostIt;
    }
    else {
        currentGoal = Goal::Patrolling;
    }

    vector<Action*> actions = planner.Plan(*this, currentGoal);

    for (Action* action : actions) {
        if (action->CanExecute(*this)) {
            action->Execute(*this);
            break;
        }
    }
}

void EnemyPatroller::setWarning(bool alert, Vector2f newtargetpos) {
    if (alert) {
        warning = alert;
        targetpos = newtargetpos;
        lookAroundTime = 0.0f;
        setAtTargetPosition(false);
        shape.setFillColor(Color::Blue);
        cout << "EnemyPatroller alerted!\n";

    }
    if (!alert) {
        shape.setFillColor(Color::Red);
    }
}


bool EnemyPatroller::atTargetPosition() const {
    return atTarget;
}

void EnemyPatroller::setAtTargetPosition(bool value) {
    atTarget = value;
}

void EnemyPatroller::reset() {
    warning = false;
    atTarget = false;
    playerDetected = false;
}

bool EnemyPatroller::isTriangleIntersectingRect(Vector2f a, Vector2f b, Vector2f c, FloatRect rect) {
    if (rect.contains(a) || rect.contains(b) || rect.contains(c)) {
        return true;
    }

    Vector2f topLeft(rect.left, rect.top);
    Vector2f topRight(rect.left + rect.width, rect.top);
    Vector2f bottomLeft(rect.left, rect.top + rect.height);
    Vector2f bottomRight(rect.left + rect.width, rect.top + rect.height);

    if (isPointInTriangle(topLeft, a, b, c) ||
        isPointInTriangle(topRight, a, b, c) ||
        isPointInTriangle(bottomLeft, a, b, c) ||
        isPointInTriangle(bottomRight, a, b, c)) {
        return true;
    }
    return false;
}

bool EnemyPatroller::isPointInTriangle(Vector2f p, Vector2f a, Vector2f b, Vector2f c) {
    float d1 = (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
    float d2 = (p.x - c.x) * (b.y - c.y) - (b.x - c.x) * (p.y - c.y);
    float d3 = (p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y);

    bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(hasNeg && hasPos);
}

VertexArray EnemyPatroller::getViewConeShape(Grid& grid) {
    VertexArray cone(TriangleFan);
    cone.append(Vertex(shape.getPosition(), Color(255, 255, 0, 150)));

    float fov = 50.0f;
    float viewDistance = 200.0f;
    int segments = 15;

    for (int i = 0; i <= segments; ++i) {
        float angle = enemyAngle - (fov / 2) + (i * (fov / segments));
        float rad = angle * (3.14159265358979323846 / 180.0f);
        Vector2f endPoint = shape.getPosition() + Vector2f(cos(rad), sin(rad)) * viewDistance;

        Vector2f step = (endPoint - shape.getPosition()) / static_cast<float>(viewDistance / 5.0f);
        Vector2f rayPos = shape.getPosition();
        for (float j = 0; j < viewDistance; j += 5.0f) {
            rayPos += step;
            int gridX = static_cast<int>(rayPos.x / CELL_SIZE);
            int gridY = static_cast<int>(rayPos.y / CELL_SIZE);
            if (!grid.getCell(gridX, gridY).walkable) {
                break;
            }
            endPoint = rayPos;
        }
        cone.append(Vertex(endPoint, Color(255, 255, 0, 150)));
    }
    return cone;
}

void EnemyPatroller::drawViewCone(RenderWindow& window, Grid& grid) {
    VertexArray cone = getViewConeShape(grid);
    window.draw(cone);
}

void EnemyPatroller::rotateTowards(const Vector2f& direction) {
    if (direction.x != 0 || direction.y != 0) {
        // Calcul de l'angle cible
        float targetAngle = atan2(direction.y, direction.x) * (180.0f / 3.14159265358979323846f);

        // Normalisation des angles pour éviter des sauts brusques
        float angleDifference = targetAngle - enemyAngle;
        if (angleDifference > 180.0f) angleDifference -= 360.0f;
        if (angleDifference < -180.0f) angleDifference += 360.0f;

        // Appliquer la vitesse de rotation max
        float rotationStep = maxRotationSpeed * deltaTime;
        if (fabs(angleDifference) <= rotationStep) {
            enemyAngle = targetAngle; // Si proche de la cible, directement assigner
        }
        else {
            enemyAngle += (angleDifference > 0 ? 1 : -1) * rotationStep;
        }
    }
}

// ===================================================================================Actions=================================================================================


bool ChasePlayer::CanExecute(const EnemyPatroller& state) {
    return state.warning && !state.atTargetPosition();
}

void ChasePlayer::Execute(EnemyPatroller& state) {
    cout << "Chasing player\n";
    Vector2f direction = state.targetpos - state.shape.getPosition();
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


bool LookAround::CanExecute(const EnemyPatroller& state) {
    return state.warning && state.atTargetPosition();
}

void LookAround::Execute(EnemyPatroller& state) {
    cout << "Looking around\n";
    
    if (state.playerDetected) {
        cout << "Player detected during LookAround, switching to Chase\n";
        state.lookAroundTime = 0.0f;
        state.warning = true;
        state.setAtTargetPosition(false);
        return;
    }
    
    state.lookAroundTime += state.deltaTime;
    if (state.lookAroundTime <= 1.5f) {
        state.enemyAngle += 100 * state.deltaTime * state.rotatedir;
    }
    else state.enemyAngle += 100 * state.deltaTime * state.rotatedir * -1;
    if (state.lookAroundTime >= 4.5f) {
        state.reset();
        state.lookAroundTime = 0.0f;
        state.rotatedir = rand() % 1;
        if (state.rotatedir == 0)state.rotatedir = -1;
    }
    

}


vector<Action*> GOAPPlanner::Plan(const EnemyPatroller& initialState, Goal goal) {
    vector<Action*> plan;

    if (goal == Goal::Chasing) {
        plan.push_back(new ChasePlayer());
    }
    else if (goal == Goal::LostIt) {
        plan.push_back(new ChasePlayer());
        plan.push_back(new LookAround());

    }
    else if (goal == Goal::Patrolling) {
        // Patrolling actions would go here (e.g., Move)
    }

    return plan;
}
