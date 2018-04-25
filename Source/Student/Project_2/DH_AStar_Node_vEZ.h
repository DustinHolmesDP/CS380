/******************************************************************************/
/*!
\file		DH_AStar_Node_vEZ.h
\project	CS380/CS580 AI Framework
\author		Dustin Holmes
\summary	A* node implementation.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

namespace AStar
{
    class NodeVEZ
    {
    public:
        NodeVEZ();
        size_t iteration;
        NodeVEZ *parent;
        float cost;
        float given;
        unsigned char row;
        unsigned char col;
        bool onOpen;
    };

    inline NodeVEZ::NodeVEZ() : iteration(0), parent(nullptr), cost(-1.0f), given(-1.0f), row(-1), col(-1), onOpen(false)
    {}
}
