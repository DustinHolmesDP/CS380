#pragma once

// forward declaration
class BehaviorNode;
class BehaviorTreePrototype;

class BehaviorTree
{
    friend class BehaviorTreePrototype;
public:
    BehaviorTree();
    ~BehaviorTree();

    void update(float dt);

    const char *get_tree_name() const;
private:
    BehaviorNode *rootNode;
    const char *treeName;
};