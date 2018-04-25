/******************************************************************************/
/*!
\file		DH_AStar_OpenList_vEZ.cpp
\project	CS380/CS580 AI Framework
\author		Dustin Holmes
\summary	A* open list implementation.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#include <pch.h>
#include "DH_Astar_OpenList_vEZ.h"

namespace AStar
{
    void OpenListVEZ::push(NodeVEZ *node)
    {
        nodes.emplace_back(node);
    }

    NodeVEZ *OpenListVEZ::pop()
    {
        #undef max
        float minCost = std::numeric_limits<float>().max();
        int minIndex = -1;
        NodeVEZ *minNode = nullptr;
        
        for (int i = 0; i < static_cast<int>(nodes.size()); ++i)
        {
            if (nodes[i]->cost < minCost)
            {
                minCost = nodes[i]->cost;
                minIndex = i;
                minNode = nodes[i];
            }
        }

        if (minIndex != -1)
        {
            nodes[minIndex] = nodes.back();
            nodes.pop_back();
        }

        return minNode;
    }

    void OpenListVEZ::reset()
    {
        nodes.clear();
    }
}
