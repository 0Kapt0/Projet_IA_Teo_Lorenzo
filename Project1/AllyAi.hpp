#ifndef ALLY_AI_HPP
#define ALLY_AI_HPP

#include "Grid.hpp"
#include <vector>
#include <memory>
#include <queue>
#include <functional>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "EnemyPatroller.hpp"
#include <map>

using namespace sf;

//États possibles des nœuds du Behavior Tree
enum class NodeState { SUCCESS, FAILURE, RUNNING };

//Classes de Behavior Tree
class BTNode {
public:
    virtual ~BTNode() = default;
    virtual NodeState execute() = 0;
};

class SequenceNode : public BTNode {
private:
    std::vector<std::unique_ptr<BTNode>> children;
public:
    void addChild(std::unique_ptr<BTNode> child) { children.push_back(std::move(child)); }
    NodeState execute() override {
        for (auto& child : children) {
            if (child->execute() == NodeState::FAILURE) {
                return NodeState::FAILURE;
            }
        }
        return NodeState::SUCCESS;
    }
};

class SelectorNode : public BTNode {
private:
    std::vector<std::unique_ptr<BTNode>> children;
public:
    void addChild(std::unique_ptr<BTNode> child) { children.push_back(std::move(child)); }
    NodeState execute() override {
        for (auto& child : children) {
            if (child->execute() == NodeState::SUCCESS) {
                return NodeState::SUCCESS;
            }
        }
        return NodeState::FAILURE;
    }
};

class ConditionNode : public BTNode {
private:
    std::function<bool()> condition;
public:
    ConditionNode(std::function<bool()> func) : condition(func) {}
    NodeState execute() override {
        return condition() ? NodeState::SUCCESS : NodeState::FAILURE;
    }
};

class ActionNode : public BTNode {
private:
    std::function<NodeState()> action;
public:
    ActionNode(std::function<NodeState()> func) : action(func) {}
    NodeState execute() override {
        return action();
    }
};

class AllyAI {
public:
    AllyAI(float x, float y, Grid& grid);
    void update(float deltaTime);
    void computePathToTarget(const Vector2f& targetPos);
    void alert(Vector2f newTarget);
    void draw(sf::RenderWindow& window);

    const RectangleShape& getShape() const { return shape; }

private:
    void moveTowardsTarget(float deltaTime);
    std::unique_ptr<BTNode> behaviorTree;
    std::queue<Vector2f> pathToTarget;
    Vector2f targetPos;
    Grid& grid;
    RectangleShape shape;
};

#endif // ALLY_AI_HPP
