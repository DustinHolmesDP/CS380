#pragma once

enum class BehaviorTreeTypes
{
    #define TREENAME(Name) Name,
    #define TREENODE(Node, Depth)
    
    #include "../Student/Project_1/Trees.def"
    NUM_ENTRIES

    #undef TREENAME
    #undef TREENODE
};

static const char *behaviorTreeNames[] = 
{
    #define TREENAME(Name) #Name,
    #define TREENODE(Node, Depth)

    #include "../Student/Project_1/Trees.def"
    "Invalid"

    #undef TREENAME
    #undef TREENODE
};

BehaviorTreeTypes string_to_behavior_tree_type(const char *name);

const char *tree_type_to_tree_name(BehaviorTreeTypes type);