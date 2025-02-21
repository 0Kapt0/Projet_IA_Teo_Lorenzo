#include "EnemyPatroller.hpp"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace sf;
using namespace std;

EnemyPatroller::EnemyPatroller(float x, float y, Vector2f point1, Vector2f point2, Vector2f point3) : point1(point1), point2(point2), point3(point3), Enemy(x, y) {
    if (!enemyTexture.loadFromFile("assets/texture/ENEMY.png")) {
        cerr << "Erreur chargement TEXTURE de la camera !" << endl;
    }

    targetpos = Vector2f(x, y);
    reset();
    shape.setTexture(&enemyTexture);
    shape.setFillColor(Color::White);
    shape.setOrigin((shape.getSize().x / 2), shape.getSize().y / 2);
}

void EnemyPatroller::update(float deltaTime, Grid& grid, Player& player) {
    this->deltaTime = deltaTime;
    shape.setRotation(enemyAngle);
    computePathToTarget(grid, targetpos);
    if (playerDetected || warning) {
        SPEED = 150;
        
    } else {
        SPEED = 100;
    }

    if (!pathToPlayer.empty()) {
        moveTowardsTarget(deltaTime);
    }

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
    for (auto it = player.cookies.begin(); it != player.cookies.end(); ) {
        for (size_t i = 1; i < cone.getVertexCount() - 1; ++i) {
            if (isTriangleIntersectingRect(cone[0].position, cone[i].position, cone[i + 1].position, (*it)->shape.getGlobalBounds())) {
                warning = true;
                playerDetected = true;
                targetpos = (*it)->shape.getPosition();
                setAtTargetPosition(false);
                break;
            }
        }
        FloatRect expandedBounds = (*it)->shape.getGlobalBounds();
        expandedBounds.left -= 15;
        expandedBounds.top -= 15;
        expandedBounds.width += 20;
        expandedBounds.height += 20;

        if (shape.getGlobalBounds().intersects(expandedBounds)) {
            it = player.cookies.erase(it);
        }
        else {
            ++it;
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
            action->Execute(*this, grid);
            break;
        }
    }
}

void EnemyPatroller::moveTowardsTarget(float deltaTime) {
    if (pathToPlayer.empty()) return;

    Vector2f currentPos = shape.getPosition();
    Vector2f targetPos = pathToPlayer.front();

    Vector2f direction = targetPos - currentPos;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0) {
        direction /= distance;
    }

    float moveStep = SPEED * deltaTime;

    if (distance < moveStep || distance < 10.0f) {
        shape.setPosition(targetPos);
        pathToPlayer.pop();
        if (pathToPlayer.empty()) {
            setAtTargetPosition(true);
        }
        return;
    }

    shape.move(direction * moveStep);
    float angle = atan2(direction.y, direction.x) * 180 / M_PI;
    shape.setRotation(angle);
    rotateTowards(direction);
}

void EnemyPatroller::computePathToTarget(Grid& grid, const Vector2f& targetPos) {
    Vector2i start(
        static_cast<int>(shape.getPosition().x / CELL_SIZE),
        static_cast<int>(shape.getPosition().y / CELL_SIZE)
    );
    Vector2i end(
        static_cast<int>(targetPos.x / CELL_SIZE),
        static_cast<int>(targetPos.y / CELL_SIZE)
    );

    if (!grid.isWalkable(end.x, end.y)) {
        //cout << "🚨 Destination bloquée, recalcul impossible !" << endl;
        return;
    }

    struct Node {
        Vector2i pos;
        float cost;
        float heuristic;
        bool operator>(const Node& other) const {
            return (cost + heuristic) > (other.cost + other.heuristic);
        }
    };

    priority_queue<Node, vector<Node>, greater<Node>> openSet;
    map<Vector2i, Vector2i, Vector2iComparator> cameFrom;
    map<Vector2i, float, Vector2iComparator> costSoFar;
    vector<Vector2i> path;

    openSet.push({ start, 0, static_cast<float>(abs(start.x - end.x) + abs(start.y - end.y)) });
    cameFrom[start] = start;
    costSoFar[start] = 0;

    vector<Vector2i> directions = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
    };

    while (!openSet.empty()) {
        Node current = openSet.top();
        openSet.pop();

        if (current.pos == end) {
            while (current.pos != start) {
                path.push_back(current.pos);
                current.pos = cameFrom[current.pos];
            }
            reverse(path.begin(), path.end());

            pathToPlayer = queue<Vector2f>();
            for (const auto& pos : path) {
                Vector2f alignedPos(
                    (pos.x * CELL_SIZE) + (CELL_SIZE / 2),
                    (pos.y * CELL_SIZE) + (CELL_SIZE / 2)
                );
                pathToPlayer.push(alignedPos);
            }
            return;
        }

        for (const auto& dir : directions) {
            Vector2i neighbor = current.pos + dir;

            if (!grid.isWalkable(neighbor.x, neighbor.y)) continue;

            float newCost = costSoFar[current.pos] + 1;
            if (costSoFar.find(neighbor) == costSoFar.end() || newCost < costSoFar[neighbor]) {
                costSoFar[neighbor] = newCost;
                float priority = newCost + static_cast<float>(abs(neighbor.x - end.x) + abs(neighbor.y - end.y));
                openSet.push({ neighbor, newCost, priority });
                cameFrom[neighbor] = current.pos;
            }
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
        /*cout << "EnemyPatroller alerted!\n"*/;

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

void EnemyPatroller::Patrolling(Grid& grid) {
    
    if (ascending) {
        switch (etape) {
        case 1:
            targetpos = point1;
            break;
        case 2:
            targetpos = point2;
            break;
        case 3:
            targetpos = point3;
            break;
        default:
            return;
        }
    }
    else {
        switch (etape) {
        case 1:
            targetpos = point3;
            break;
        case 2:
            targetpos = point2;
            break;
        case 3:
            targetpos = point1;
            break;
        default:
            return;
        }
    }

    float distanceToTarget = sqrt(pow(shape.getPosition().x - targetpos.x, 2) +
        pow(shape.getPosition().y - targetpos.y, 2));
    if (distanceToTarget < 50.0f) { 
        if (ascending) {
            if (etape == 3) {
                ascending = false;
            }
            else {
                ++etape;
            }
        }
        else {
            if (etape == 1) {
                ascending = true; 
            }
            else {
                --etape;
            }
        }

        setAtTargetPosition(true);
        return;
    }

 
    computePathToTarget(grid, targetpos);
}


// ===================================================================================Actions=================================================================================

bool ChasePlayer::CanExecute(const EnemyPatroller& state) {
    return state.warning && !state.atTargetPosition();
}

void ChasePlayer::Execute(EnemyPatroller& state, Grid& grid) {
    /*cout << "Chasing player\n";*/
    Vector2f direction = state.targetpos - state.shape.getPosition();
    float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (magnitude <= 35.0f) {
        state.setAtTargetPosition(true);
    }
    else {
        state.rotateTowards(direction);
    }
}


bool LookAround::CanExecute(const EnemyPatroller& state) {
    return state.warning && state.atTargetPosition();
}

void LookAround::Execute(EnemyPatroller& state, Grid& grid) {
    /* cout << "Looking around\n"*/;

    if (state.playerDetected) {
        /*cout << "Player detected during LookAround, switching to Chase\n"*/;
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

bool Patrol::CanExecute(const EnemyPatroller& state) {
    return true;
}

void Patrol::Execute(EnemyPatroller& state, Grid& grid) {
    state.Patrolling(grid);
}


vector<Action*> GOAPPlanner::Plan(const EnemyPatroller& initialState, Goal Goal) {
    vector<Action*> plan;

    if (Goal == Goal::Chasing) {
        plan.push_back(new ChasePlayer());
    }
    else if (Goal == Goal::LostIt) {
        plan.push_back(new ChasePlayer());
        plan.push_back(new LookAround());

    }
    else if (Goal == Goal::Patrolling) {
        plan.push_back(new Patrol());
    }

    return plan;
}
