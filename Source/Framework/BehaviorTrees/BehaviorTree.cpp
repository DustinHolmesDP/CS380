#include <pch.h>
#include "BehaviorTree.h"
#include "BehaviorNode.h"

BehaviorTree::BehaviorTree() : rootNode(nullptr), treeName(nullptr)
{}

BehaviorTree::~BehaviorTree()
{
    delete rootNode;
}

void BehaviorTree::update(float dt)
{
    rootNode->tick(dt);

    // if the root node is complete in any way, restart it
    if (rootNode->is_running() == false)
    {
        rootNode->set_status(NodeStatus::READY);
    }
}

const char *BehaviorTree::get_tree_name() const
{
    return treeName;
}
