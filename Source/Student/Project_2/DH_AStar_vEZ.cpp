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
#include <pch.h>
#include "DH_AStar_vEZ.h"
#include <algorithm>
#undef min
#undef max
namespace AStar
{
    DH_AStarVEZ::DH_AStarVEZ() : iteration(0), goal(nullptr), start(nullptr), heuristicWeight(1.0f), heuristicType(0), smoothing(false), rubberBanding(false), straightLine(false), singleStep(false)
    {
        for (int row = 0; row < 40; ++row)
        {
            for (int col = 0; col < 40; ++col)
            {
                nodes[row][col].row = row;
                nodes[row][col].col = col;
            }
        }
    }

    void DH_AStarVEZ::set_positions(int tinyR, int tinyC, int goalR, int goalC, int heur, float weight)
    {
        goal = &nodes[goalR][goalC];
        heuristicType = heur;
        heuristicWeight = weight;
        ++iteration;

        start = &nodes[tinyR][tinyC];
        start->onOpen = true;
        start->iteration = iteration;
        start->given = 0.0f;
        start->cost = calc_heuristic(start);
        start->parent = nullptr;

        openList.reset();
        openList.push(start);
    }

    void DH_AStarVEZ::set_options(bool smooth, bool rubber, bool straight, bool single, bool debug)
    {
        smoothing = smooth;
        rubberBanding = rubber;
        straightLine = straight;
        singleStep = single;
        debugDraw = debug;
    }

    bool DH_AStarVEZ::process(WaypointList &list)
    {
        list.clear();

        if (straightLine == true)
        {
            if (perform_straight_line(list) == true)
            {
                if (smoothing == true)
                {
                    perform_repopulation(list);
                    perform_smoothing(list);
                }

                return true;
            }
        }

        const bool result = find_path(list);

        if (result == true)
        {
            if (rubberBanding == true)
            {
                perform_rubberbanding(list);
            }

            if (smoothing == true)
            {
                if (rubberBanding == true)
                {
                    perform_repopulation(list);
                }

                perform_smoothing(list);
            }
        }

        return result;
    }

    bool DH_AStarVEZ::find_path(WaypointList &list)
    {
        auto &t = *terrain;
        const int width = t.get_map_width();

        while (true)
        {
            auto popResult = openList.pop();

            if (popResult == nullptr)
            {
                build_empty_path(list);
                return true;
            }

            popResult->onOpen = false;

            if (debugDraw == true)
            {
                t.set_color(popResult->row, popResult->col, Colors::Yellow);
            }

            if (popResult == goal)
            {
                build_path(list);
                return true;
            }

            const unsigned char above = popResult->row - 1;
            const unsigned char below = popResult->row + 1;
            const unsigned char left = popResult->col - 1;
            const unsigned char right = popResult->col + 1;
            const float orthoGiven = popResult->given + 1.0f;
            static const float sq2 = std::sqrt(2.0f);
            const float diagGiven = popResult->given + sq2;

            
            const bool aValid = above < width && (t.is_wall(above, popResult->col) == false);
            const bool lValid = left < width && (t.is_wall(popResult->row, left) == false);
            const bool rValid = right < width && (t.is_wall(popResult->row, right) == false);
            const bool bValid = below < width && (t.is_wall(below, popResult->col) == false);
            const bool alValid = above < width && left < width && (t.is_wall(above, left) == false) && aValid && lValid;
            const bool arValid = above < width && right < width && (t.is_wall(above, right) == false) && aValid && rValid;
            const bool blValid = below < width && left < width && (t.is_wall(below, left) == false) && bValid && lValid;
            const bool brValid = below < width && right < width && (t.is_wall(below, right) == false) && bValid && rValid;
            
            if (alValid) evaluate(popResult, diagGiven, above, left);
            if (aValid) evaluate(popResult, orthoGiven, above, popResult->col);
            if (arValid) evaluate(popResult, diagGiven, above, right);
            if (lValid) evaluate(popResult, orthoGiven, popResult->row, left);
            if (rValid) evaluate(popResult, orthoGiven, popResult->row, right);
            if (blValid) evaluate(popResult, diagGiven, below, left);
            if (bValid) evaluate(popResult, orthoGiven, below, popResult->col);
            if (brValid) evaluate(popResult, diagGiven, below, right);

            if (singleStep == true)
            {
                return false;
            }
        }
    }

    void DH_AStarVEZ::evaluate(NodeVEZ *parent, float newGiven, unsigned char row, unsigned char col)
    {
        auto &t = *terrain;

        if (row < t.get_map_width() && col < t.get_map_width())
        {
            if (t.is_wall(row, col) == false)
            {
                auto &node = nodes[row][col];

                if (node.iteration != iteration)
                {
                    node.iteration = iteration;
                    node.parent = parent;
                    node.given = newGiven;
                    node.cost = newGiven + calc_heuristic(&node);
                    node.onOpen = true;

                    if (debugDraw == true)
                    {
                        t.set_color(node.row, node.col, Colors::Blue);
                    }
                    openList.push(&node);
                }
                else if (node.onOpen == true && newGiven < node.given)
                {
                    node.parent = parent;
                    const float heuristic = node.cost - node.given;
                    node.given = newGiven;
                    node.cost = newGiven + heuristic;
                }
            }
        }
    }

    void DH_AStarVEZ::build_path(WaypointList &list)
    {
        auto &t = *terrain;
        list.emplace_front(t.get_world_position(goal->row, goal->col));

        NodeVEZ *nextNode = goal->parent;
        while (nextNode != nullptr)
        {
            list.emplace_front(t.get_world_position(nextNode->row, nextNode->col));
            nextNode = nextNode->parent;
        }
    }

    void DH_AStarVEZ::build_empty_path(WaypointList &list)
    {
        list.emplace_front(terrain->get_world_position(start->row, start->col));
    }

    bool DH_AStarVEZ::perform_straight_line(WaypointList &list)
    {
        auto &t = *terrain;

        if (walls_in_bounds(start->row, start->col, goal->row, goal->col) == false)
        {
            list.emplace_front(t.get_world_position(goal->row, goal->col));
            list.emplace_front(t.get_world_position(start->row, start->col));

            return true;
        }

        return false;
    }

    void DH_AStarVEZ::perform_rubberbanding(WaypointList &list)
    {
        if (list.size() < 3)
        {
            return;
        }

        auto back = list.begin();
        
        auto mid = back;
        ++mid;

        auto front = mid;
        ++front;

        while (front != list.end())
        {
            if (walls_in_bounds(*front, *back) == false)
            {
                list.erase(mid);

                mid = front;
                ++front;
            }
            else
            {
                back = mid;
                mid = front;
                ++front;
            }
        }
    }

    void DH_AStarVEZ::perform_repopulation(WaypointList &list)
    {
        auto &t = *terrain;

        const float stepSize = (100.0f / static_cast<float>(t.get_map_width())) * 1.5f;

        auto current = list.begin();
        auto next = current;
        ++next;

        while (next != list.end())
        {
            while (true)
            {
                auto delta = *next - *current;
                const float length = delta.Length();

                if (length > stepSize)
                {
                    const auto midPoint = *current + (delta * 0.5f);

                    next = list.insert(next, midPoint);
                }
                else
                {
                    break;
                }
            }

            current = next;
            ++next;
        }
    }

    void DH_AStarVEZ::perform_smoothing(WaypointList &list)
    {
        if (list.size() >= 4)
        {
            auto p1 = list.begin();
            auto p2 = p1;
            auto p3 = p2;
            ++p3;
            auto p4 = p3;
            ++p4;

            smooth_section(list, p1, p2, p3, p4);
            p2 = p3;
            p3 = p4;
            ++p4;

            while (p4 != list.end())
            {
                smooth_section(list, p1, p2, p3, p4);
                p1 = p2;
                p2 = p3;
                p3 = p4;
                ++p4;
            }

            smooth_section(list, p1, p2, p3, p3);
        }
        else if (list.size() == 3)
        {
            auto p1 = list.begin();
            auto p2 = p1;
            ++p2;
            auto p3 = p2;
            ++p3;

            smooth_section(list, p1, p1, p2, p3);
            smooth_section(list, p1, p2, p3, p3);
        }
        else if (list.size() == 2)
        {
            auto p1 = list.begin();
            auto p2 = p1;
            ++p2;

            smooth_section(list, p1, p1, p2, p2);
        }
    }

    void DH_AStarVEZ::smooth_section(WaypointList &list, WaypointList::iterator p1, WaypointList::iterator p2, WaypointList::iterator p3, WaypointList::iterator p4)
    {
        Vec3 newPoint = Vec3::CatmullRom(*p1, *p2, *p3, *p4, 0.75f);

        auto insertPoint = list.insert(p3, newPoint);

        newPoint = Vec3::CatmullRom(*p1, *p2, *p3, *p4, 0.5f);

        insertPoint = list.insert(insertPoint, newPoint);

        newPoint = Vec3::CatmullRom(*p1, *p2, *p3, *p4, 0.25f);

        list.insert(insertPoint, newPoint);
    }

    float DH_AStarVEZ::calc_heuristic(NodeVEZ *node) const
    {
        const float xDiff = (node->col > goal->col) ? static_cast<float>(node->col - goal->col) : static_cast<float>(goal->col - node->col);
        const float yDiff = (node->row > goal->row) ? static_cast<float>(node->row - goal->row) : static_cast<float>(goal->row - node->row);
        static const float sq2 = std::sqrt(2.0f);

        switch (heuristicType)
        {
        case 1: // octile
            if (xDiff < yDiff)
            {
                return (xDiff * sq2 + (yDiff - xDiff)) * heuristicWeight;
            }
            else if (yDiff < xDiff)
            {
                return (yDiff * sq2 + (xDiff - yDiff)) * heuristicWeight;
            }
            else
            {
                return xDiff * sq2 * heuristicWeight;
            }
        case 2: // chebyshev
            return std::max(xDiff, yDiff) * heuristicWeight;
        case 3: // manhattan
            return (xDiff + yDiff) * heuristicWeight;
        default: // euclidean
            return std::sqrt(xDiff * xDiff + yDiff * yDiff) * heuristicWeight;
        }
    }

    bool DH_AStarVEZ::walls_in_bounds(int r0, int c0, int r1, int c1)
    {
        auto &t = *terrain;

        const int minRow = (r0 < r1) ? r0 : r1;
        const int maxRow = (r0 < r1) ? r1 : r0;
        const int minCol = (c0 < c1) ? c0 : c1;
        const int maxCol = (c0 < c1) ? c1 : c0;

        for (int row = minRow; row <= maxRow; ++row)
        {
            for (int col = minCol; col <= maxCol; ++col)
            {
                if (t.is_wall(row, col) == true)
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool DH_AStarVEZ::walls_in_bounds(Vec3 &p0, Vec3 &p1)
    {
        auto &t = *terrain;;

        const auto g0 = t.get_grid_position(p0);
        const auto g1 = t.get_grid_position(p1);

        return walls_in_bounds(g0.row, g0.col, g1.row, g1.col);
    }
}
