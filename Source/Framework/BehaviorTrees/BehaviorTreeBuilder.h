#pragma once
#include "TreeInfo.h"
#include "BehaviorTreePrototype.h"
#include <vector>

class BehaviorTreeBuilder
{
public:
    bool initialize();
    void shutdown();

    void build_tree(BehaviorTreeTypes type, BehaviorAgent *agent);

private:
    std::vector<BehaviorTreePrototype> prototypes;

    bool deserialize_tree(const std::experimental::filesystem::path &filepath);

    BehaviorTreeTypes deserialize_tree_type(FILE *file);
    bool deserialize_node(FILE *file, NodeTypes &type, int &depth);
};