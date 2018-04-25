/******************************************************************************/
/*!
\file		DH_AStar_OpenList_vEZ.h
\project	CS380/CS580 AI Framework
\author		Dustin Holmes
\summary	A* open list implementation.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#include "DH_AStar_Node_vEZ.h"
#include <vector>

namespace AStar
{
    class OpenListVEZ
    {
    public:
        void push(NodeVEZ *node);
        NodeVEZ *pop();
        void reset();
    private:
        std::vector<NodeVEZ *> nodes;
    };
}