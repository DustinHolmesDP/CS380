/******************************************************************************/
/*!
\file		DH_AStar_vEZ.h
\project	CS380/CS580 AI Framework
\author		Dustin Holmes
\summary	A* implementation.

Copyright (C) 2016 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once
#include "DH_Astar_OpenList_vEZ.h"
#include "Misc/PathfindingDetails.hpp"

namespace AStar
{
    class DH_AStarVEZ
    {
    public:
        DH_AStarVEZ();

        void set_positions(int tinyR, int tinyC, int goalR, int goalC, int heur, float weight);
        void set_options(bool smooth, bool rubber, bool straight, bool single, bool debug);
        bool process(WaypointList &list);

    private:
        NodeVEZ nodes[40][40];
        OpenListVEZ openList;
        size_t iteration;
        NodeVEZ *goal;
        NodeVEZ *start;
        float heuristicWeight;
        int heuristicType;
        bool smoothing;
        bool rubberBanding;
        bool straightLine;
        bool singleStep;
        bool debugDraw;

        bool find_path(WaypointList &list);
        void evaluate(NodeVEZ *parent, float newGiven, unsigned char row, unsigned char col);
        void build_path(WaypointList &list);
        void build_empty_path(WaypointList &list);

        bool perform_straight_line(WaypointList &list);
        void perform_rubberbanding(WaypointList &list);
        void perform_repopulation(WaypointList &list);
        static void perform_smoothing(WaypointList &list);
        static void smooth_section(WaypointList &list, WaypointList::iterator p1, WaypointList::iterator p2, WaypointList::iterator p3, WaypointList::iterator p4);

        float calc_heuristic(NodeVEZ *node) const;

        bool walls_in_bounds(int r0, int c0, int r1, int c1);
        bool walls_in_bounds(Vec3 &p0, Vec3 &p1);
    };
}