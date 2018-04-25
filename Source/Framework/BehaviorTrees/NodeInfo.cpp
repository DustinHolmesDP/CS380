#include <pch.h>
#include "NodeInfo.h"

BehaviorNode *node_type_to_node(NodeTypes type)
{
    BehaviorNode *result = nullptr;

    auto target = nodePrototypes[static_cast<size_t>(type)];

    if (target != nullptr)
    {
        result = target->clone();
    }

    return result;
}

NodeTypes string_to_node_type(const char *nodeName)
{
    for (size_t i = 0; i < static_cast<size_t>(NodeTypes::NUM_ENTRIES); ++i)
    {
        if (std::strcmp(nodeNames[i], nodeName) == 0)
        {
            return static_cast<NodeTypes>(i);
        }
    }

    return NodeTypes::NUM_ENTRIES;
}

const char *node_type_to_name(NodeTypes type)
{
    return nodeNames[static_cast<size_t>(type)];
}

const char *node_type_to_summary(NodeTypes type)
{
    return nodeSummaries[static_cast<size_t>(type)];
}