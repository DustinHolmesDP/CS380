#pragma once
#include "NodeInfo.h"
#include <vector>

class BehaviorAgent;

class BehaviorTreePrototype
{
public:
    BehaviorTreePrototype();

    void build_tree(BehaviorAgent *agent);

    void add_node(NodeTypes type, int depth);

    void set_tree_name(const char *name);

private:
    struct NodeRep
    {
        NodeRep(NodeTypes type, int depth);
        NodeTypes type;
        int depth;
        std::vector<NodeRep> children;
    };

    NodeRep rootNode;
    NodeRep *previousNode;
    const char *treeName;

    void recursive_add(NodeRep &rep, BehaviorNode *node);
};