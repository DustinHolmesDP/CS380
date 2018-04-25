#include <pch.h>
#include "Terrain/TerrainAnalysis.h"
#include "Terrain/MapMath.h"
#include "Agent/AStarAgent.h"
#include "Terrain/MapLayer.h"
#include "Projects/ProjectThree.h"

#include <iostream>

bool ProjectThree::implemented_fog_of_war() const // extra credit
{
    return false;
}

float distance_to_closest_wall(int row, int col)
{
    /*
        Check the euclidean distance from the given cell to every other wall cell,
        as well as the distance to the edges (they count as walls for this),
        and return the smallest distance.  Make use of the is_wall member function
        in the global terrain to determine if a cell is a wall or not.
    */

    // WRITE YOUR CODE HERE
    auto &t = *terrain;
    const int maxIndex = terrain->get_map_width() - 1;

    if (row == 0 || row == maxIndex || col == 0 || col == maxIndex)
    {
        return 1.0f;
    }

    float closest = std::numeric_limits<float>().max();

    for (int r = 0; r <= maxIndex; ++r)
    {
        for (int c = 0; c <= maxIndex; ++c)
        {
            if (r == row && c == col)
            {
                continue;
            }

            if (t.is_wall(r, c) == true)
            {
                const float rDelta = static_cast<float>(r) - static_cast<float>(row);
                const float cDelta = static_cast<float>(c) - static_cast<float>(col);

                const float distance = std::sqrt(rDelta * rDelta + cDelta * cDelta);
                closest = std::min(closest, distance);

                if (closest == 1.0f)
                {
                    return 1.0f;
                }
            }
        }
    }

    return closest;
    //return 0.0f; // REPLACE THIS
}

bool is_clear_path(int row0, int col0, int row1, int col1)
{
    /*
        Two cells (row0, col0) and (row1, col1) are visible to each other if a line
        between their centerpoints doesn't intersect the four boundary lines of every
        wall cell.  You should puff out the four boundary lines by a very tiny amount
        so that a diagonal line passing by the corner will intersect it.  Make use of the
        is_wall member function in the global terrain to determine if a cell is a wall or not.
    */

    // WRITE YOUR CODE HERE
    auto &t = *terrain;

    const float baseWidth = globalScalar * 0.5f;
    const float expansionAmount = baseWidth / 100.0f;
    const float cellWidth = (baseWidth + expansionAmount) * sqrt(2.0f);

    const Vec2 ulo(-cellWidth, cellWidth);
    const Vec2 uro(cellWidth, cellWidth);
    const Vec2 blo(-cellWidth, -cellWidth);
    const Vec2 bro(cellWidth, -cellWidth);

    const auto pos1_3 = t.get_world_position(row0, col0);
    const auto pos2_3 = t.get_world_position(row1, col1);

    const Vec2 pos1(pos1_3.x, pos1_3.z);
    const Vec2 pos2(pos2_3.x, pos2_3.z);

    const int minR = (row0 < row1) ? row0 : row1;
    const int maxR = (row0 < row1) ? row1 : row0;
    const int minC = (col0 < col1) ? col0 : col1;
    const int maxC = (col0 < col1) ? col1 : col0;

    for (int row = minR; row <= maxR; ++row)
    {
        for (int col = minC; col <= maxC; ++col)
        {
            if ((row == row0 && col == col0) || (row == row1 && col == col1))
            {
                continue;
            }

            if (t.is_wall(row, col) == true)
            {
                const auto center_3 = t.get_world_position(row, col);
                const Vec2 center(center_3.x, center_3.z);
                const auto ul = center + ulo;
                const auto ur = center + uro;
                const auto bl = center + blo;
                const auto br = center + bro;

                if (line_intersect(pos1, pos2, ul, ur) == true)
                {
                    return false;
                }

                if (line_intersect(pos1, pos2, ur, br) == true)
                {
                    return false;
                }

                if (line_intersect(pos1, pos2, br, bl) == true)
                {
                    return false;
                }

                if (line_intersect(pos1, pos2, bl, ul) == true)
                {
                    return false;
                }
            }
        }
    }

    return true;

    return false; // REPLACE THIS
}

void analyze_openness(MapLayer<float> &layer)
{
    /*
        Mark every cell in the given layer with the value 1 / (d * d),
        where d is the distance to the closest wall or edge.  Make use of the
        distance_to_closest_wall helper function.
    */

    // WRITE YOUR CODE HERE
    auto &t = *terrain;

    const int maxIndex = t.get_map_width() - 1;

    for (int row = 0; row <= maxIndex; ++row)
    {
        for (int col = 0; col <= maxIndex; ++col)
        {
            if (t.is_wall(row, col) == false)
            {
                const float dist = distance_to_closest_wall(row, col);

                layer.set_value(row, col, 1.0f / (dist * dist));
            }
        }
    }
}

void analyze_visibility(MapLayer<float> &layer)
{
    /*
        Mark every cell in the given layer with the number of cells that
        are visible to it, divided by 160 (a magic number that looks good).  Make sure
        to cap the value at 1.0 as well.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
        helper function.
    */

    // WRITE YOUR CODE HERE
    auto &t = *terrain;
    int m_width = t.get_map_width();
    for (int sourceRow = 0; sourceRow < m_width; ++sourceRow)
    {
        for (int sourceCol = 0; sourceCol < m_width; ++sourceCol)
        {
            if (t.is_wall(sourceRow, sourceCol) == false)
            {
                int numVisible = 0;

                for (int destRow = 0; destRow < m_width; ++destRow)
                {
                    for (int destCol = 0; destCol < m_width; ++destCol)
                    {
                        if (destRow == sourceRow && destCol == sourceCol)
                        {
                            continue;
                        }

                        if (is_clear_path(sourceRow, sourceCol, destRow, destCol) == true)
                        {
                            ++numVisible;
                        }
                    }
                }

                const float result = std::min(static_cast<float>(numVisible) / 100.0f, 1.0f);

                layer.set_value(sourceRow, sourceCol, result);
            }
        }
    }
}

void analyze_visble_to_cell(MapLayer<float> &layer, int row, int col)
{
    /*
        For every cell in the given layer mark it with 1.0
        if it is visible to the given cell, 0.5 if it isn't visible but is next to a visible cell,
        or 0.0 otherwise.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
        helper function.
    */

    // WRITE YOUR CODE HERE
    const int m_width = terrain->get_map_width();

    for (int r = 0; r < m_width; ++r)
    {
        for (int c = 0; c < m_width; ++c)
        {
            if (is_clear_path(row, col, r, c) == true)
            {
                layer.set_value(r, c, 1.0f);
            }
            else
            {
                layer.set_value(r, c, 0.0f);
            }
        }
    }

    for (int r = 0; r < m_width; ++r)
    {
        for (int c = 0; c < m_width; ++c)
        {
            if (layer.get_value(r, c) == 0.0f)
            {
                const int above = r - 1;
                const int below = r + 1;
                const int left = c - 1;
                const int right = c + 1;

                const bool alWall = (above >= 0 && left >= 0) ? terrain->is_wall(above, left) : true;
                const bool aWall = (above >= 0) ? terrain->is_wall(above, c) : true;
                const bool arWall = (above >= 0 && right < m_width) ? terrain->is_wall(above, right) : true;
                const bool lWall = (left >= 0) ? terrain->is_wall(r, left) : true;
                const bool rWall = (right < m_width) ? terrain->is_wall(r, right) : true;
                const bool blWall = (below < m_width && left >= 0) ? terrain->is_wall(below, left) : true;
                const bool bWall = (below < m_width) ? terrain->is_wall(below, c) : true;
                const bool brWall = (below < m_width && right < m_width) ? terrain->is_wall(below, right) : true;

                if (aWall == false)
                {
                    if (layer.get_value(above, c) == 1.0f)
                    {
                        layer.set_value(r, c, 0.5f);
                        continue;
                    }
                }

                if (lWall == false)
                {
                    if (layer.get_value(r, left) == 1.0f)
                    {
                        layer.set_value(r, c, 0.5f);
                        continue;
                    }
                }

                if (rWall == false)
                {
                    if (layer.get_value(r, right) == 1.0f)
                    {
                        layer.set_value(r, c, 0.5f);
                        continue;
                    }
                }

                if (bWall == false)
                {
                    if (layer.get_value(below, c) == 1.0f)
                    {
                        layer.set_value(r, c, 0.5f);
                        continue;
                    }
                }

                if ((aWall == false || lWall == false) && alWall == false)
                {
                    if (layer.get_value(above, left) == 1.0f)
                    {
                        layer.set_value(r, c, 0.5f);
                        continue;
                    }
                }

                if ((aWall == false || rWall == false) && arWall == false)
                {
                    if (layer.get_value(above, right) == 1.0f)
                    {
                        layer.set_value(r, c, 0.5f);
                        continue;
                    }
                }

                if ((bWall == false || rWall == false) && brWall == false)
                {
                    if (layer.get_value(below, right) == 1.0f)
                    {
                        layer.set_value(r, c, 0.5f);
                        continue;
                    }
                }

                if ((bWall == false || lWall == false) && blWall == false)
                {
                    if (layer.get_value(below, left) == 1.0f)
                    {
                        layer.set_value(r, c, 0.5f);
                        continue;
                    }
                }
            }
        }
    }
}

void analyze_agent_vision(MapLayer<float> &layer, const Agent *agent)
{
    /*
        For every cell in the given layer that is visible to the given agent,
        mark it as 1.0, otherwise don't change the cell's current value.

        You must consider the direction the agent is facing.  All of the agent data is
        in three dimensions, but to simplify you should operate in two dimensions, the XZ plane.

        Give the agent a field of view slighter larger than 180 degrees.

        Two cells are visible to each other if a line between their centerpoints doesn't
        intersect the four boundary lines of every wall cell.  Make use of the is_clear_path
        helper function.
    */

    // WRITE YOUR CODE HERE
    const auto pos = agent->get_position();
    const auto grid = terrain->get_grid_position(pos);
    const int m_width = terrain->get_map_width();

    for (int row = 0; row < m_width; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            auto delta = terrain->get_world_position(row, col) - pos;
            delta.Normalize();

            const float dot = delta.Dot(agent->get_forward_vector());

            if (dot > -0.1f)
            {
                if (is_clear_path(grid.row, grid.col, row, col) == true)
                {
                    layer.set_value(row, col, 1.0f);
                }
            }
        }
    }
}

void propagate_solo_occupancy(MapLayer<float> &layer, float decay, float growth)
{
    /*
        For every cell in the given layer:

            1) Get the value of each neighbor and apply decay factor
            2) Keep the highest value from step 1
            3) Linearly interpolate from the cell's current value to the value from step 2
               with the growing factor as a coefficient.  Make use of the lerp helper function.
            4) Store the value from step 3 in a temporary layer.
               A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

        After every cell has been processed into the temporary layer, write the temporary layer into
        the given layer;
    */
    
    // WRITE YOUR CODE HERE
    const float orthoExp = exp(-1.0f * decay);
    const float diagExp = exp(-std::sqrt(2.0f) * decay);
    const int m_width = terrain->get_map_width();
    float temp[40][40] = { 0.0f };

    for (int row = 0; row < m_width; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            if (terrain->is_wall(row, col) == true)
                continue;

            if (layer.get_value(row, col) < 0.0f)
            {
                temp[row][col] = layer.get_value(row, col);
                continue;
            }

            const int above = row - 1;
            const int below = row + 1;
            const int left = col - 1;
            const int right = col + 1;

            const bool alWall = (above >= 0 && left >= 0) ? terrain->is_wall(above, left) : true;
            const bool aWall = (above >= 0) ? terrain->is_wall(above, col) : true;
            const bool arWall = (above >= 0 && right < m_width) ? terrain->is_wall(above, right) : true;
            const bool lWall = (left >= 0) ? terrain->is_wall(row, left) : true;
            const bool rWall = (right < m_width) ? terrain->is_wall(row, right) : true;
            const bool blWall = (below < m_width && left >= 0) ? terrain->is_wall(below, left) : true;
            const bool bWall = (below < m_width) ? terrain->is_wall(below, col) : true;
            const bool brWall = (below < m_width && right < m_width) ? terrain->is_wall(below, right) : true;

            float maxInfluence = std::numeric_limits<float>().lowest();

            if (aWall == false)
            {
                if (layer.get_value(above, col) > 0.0f)
                {
                    const float newInfluence = layer.get_value(above, col) * orthoExp;
                    maxInfluence = std::max(maxInfluence, newInfluence);
                }
            }

            if (lWall == false)
            {
                if (layer.get_value(row, left) > 0.0f)
                {
                    const float newInfluence = layer.get_value(row, left) * orthoExp;
                    maxInfluence = std::max(maxInfluence, newInfluence);
                }
            }

            if (rWall == false)
            {
                if (layer.get_value(row, right) > 0.0f)
                {
                    const float newInfluence = layer.get_value(row, right) * orthoExp;
                    maxInfluence = std::max(maxInfluence, newInfluence);
                }
            }

            if (bWall == false)
            {
                if (layer.get_value(below, col) > 0.0f)
                {
                    const float newInfluence = layer.get_value(below, col) * orthoExp;
                    maxInfluence = std::max(maxInfluence, newInfluence);
                }
            }

            if ((aWall == false || lWall == false) && alWall == false)
            {
                if (layer.get_value(above, left) > 0.0f)
                {
                    const float newInfluence = layer.get_value(above, left) * diagExp;
                    maxInfluence = std::max(maxInfluence, newInfluence);
                }
            }

            if ((aWall == false || rWall == false) && arWall == false)
            {
                if (layer.get_value(above, right) > 0.0f)
                {
                    const float newInfluence = layer.get_value(above, right) * diagExp;
                    maxInfluence = std::max(maxInfluence, newInfluence);
                }
            }

            if ((bWall == false || rWall == false) && brWall == false)
            {
                if (layer.get_value(below, right) > 0.0f)
                {
                    const float newInfluence = layer.get_value(below, right) * diagExp;
                    maxInfluence = std::max(maxInfluence, newInfluence);
                }
            }

            if ((bWall == false || lWall == false) && blWall == false)
            {
                if (layer.get_value(below, left) > 0.0f)
                {
                    const float newInfluence = layer.get_value(below, left) * diagExp;
                    maxInfluence = std::max(maxInfluence, newInfluence);
                }
            }

            if (maxInfluence != std::numeric_limits<float>().lowest())
            {
                temp[row][col] = lerp(layer.get_value(row, col), maxInfluence, growth);
            }
            else
            {
                temp[row][col] = layer.get_value(row, col);
            }
        }
    }

    for (int row = 0; row < m_width; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            layer.set_value(row, col, temp[row][col]);
        }
    }
}

void propagate_dual_occupancy(MapLayer<float> &layer, float decay, float growth)
{
    /*
        Similar to the solo version, but the values range from -1.0 to 1.0, instead of 0.0 to 1.0

        For every cell in the given layer:

        1) Get the value of each neighbor and apply decay factor
        2) Keep the highest ABSOLUTE value from step 1
        3) Linearly interpolate from the cell's current value to the value from step 2
           with the growing factor as a coefficient.  Make use of the lerp helper function.
        4) Store the value from step 3 in a temporary layer.
           A float[40][40] will suffice, no need to dynamically allocate or make a new MapLayer.

        After every cell has been processed into the temporary layer, write the temporary layer into
        the given layer;
    */

    // WRITE YOUR CODE HERE
    const float orthoExp = exp(-1.0f * decay);
    const float diagExp = exp(-std::sqrt(2.0f) * decay);
    const int m_width = terrain->get_map_width();
    float temp[40][40] = { 0.0f };

    for (int row = 0; row < m_width; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            if (terrain->is_wall(row, col) == true)
                continue;

            const int above = row - 1;
            const int below = row + 1;
            const int left = col - 1;
            const int right = col + 1;

            const bool alWall = (above >= 0 && left >= 0) ? terrain->is_wall(above, left) : true;
            const bool aWall = (above >= 0) ? terrain->is_wall(above, col) : true;
            const bool arWall = (above >= 0 && right < m_width) ? terrain->is_wall(above, right) : true;
            const bool lWall = (left >= 0) ? terrain->is_wall(row, left) : true;
            const bool rWall = (right < m_width) ? terrain->is_wall(row, right) : true;
            const bool blWall = (below < m_width && left >= 0) ? terrain->is_wall(below, left) : true;
            const bool bWall = (below < m_width) ? terrain->is_wall(below, col) : true;
            const bool brWall = (below < m_width && right < m_width) ? terrain->is_wall(below, right) : true;

            float maxInfluence = 0.0f;

            if (aWall == false)
            {
                const float newInfluence = layer.get_value(above, col) * orthoExp;
                if (std::abs(newInfluence) > std::abs(maxInfluence))
                {
                    maxInfluence = newInfluence;
                }
            }

            if (lWall == false)
            {
                const float newInfluence = layer.get_value(row, left) * orthoExp;
                if (std::abs(newInfluence) > std::abs(maxInfluence))
                {
                    maxInfluence = newInfluence;
                }
            }

            if (rWall == false)
            {
                const float newInfluence = layer.get_value(row, right) * orthoExp;
                if (std::abs(newInfluence) > std::abs(maxInfluence))
                {
                    maxInfluence = newInfluence;
                }
            }

            if (bWall == false)
            {
                const float newInfluence = layer.get_value(below, col) * orthoExp;
                if (std::abs(newInfluence) > std::abs(maxInfluence))
                {
                    maxInfluence = newInfluence;
                }
            }

            if ((aWall == false || lWall == false) && alWall == false)
            {
                const float newInfluence = layer.get_value(above, left) * diagExp;
                if (std::abs(newInfluence) > std::abs(maxInfluence))
                {
                    maxInfluence = newInfluence;
                }
            }

            if ((aWall == false || rWall == false) && arWall == false)
            {
                const float newInfluence = layer.get_value(above, right) * diagExp;
                if (std::abs(newInfluence) > std::abs(maxInfluence))
                {
                    maxInfluence = newInfluence;
                }
            }

            if ((bWall == false || rWall == false) && brWall == false)
            {
                const float newInfluence = layer.get_value(below, right) * diagExp;
                if (std::abs(newInfluence) > std::abs(maxInfluence))
                {
                    maxInfluence = newInfluence;
                }
            }

            if ((bWall == false || lWall == false) && blWall == false)
            {
                const float newInfluence = layer.get_value(below, left) * diagExp;
                if (std::abs(newInfluence) > std::abs(maxInfluence))
                {
                    maxInfluence = newInfluence;
                }
            }

            temp[row][col] = lerp(layer.get_value(row, col), maxInfluence, growth);
        }
    }

    for (int row = 0; row < m_width; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            layer.set_value(row, col, temp[row][col]);
        }
    }
}

void normalize_solo_occupancy(MapLayer<float> &layer)
{
    /*
        Determine the maximum value in the given layer, and then divide the value
        for every cell in the layer by that amount.  This will keep the values in the
        range of [0, 1].
    */

    // WRITE YOUR CODE HERE
    float maxInfluence = -1.0f;
    const int m_width = terrain->get_map_width();

    for (int row = 0; row < m_width; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            const float val = layer.get_value(row, col);
            if (val > 0.0f)
            {
                maxInfluence = std::max(maxInfluence, val);
            }
        }
    }

    if (maxInfluence > 0.0f)
    {
        for (int row = 0; row < m_width; ++row)
        {
            for (int col = 0; col < m_width; ++col)
            {
                const float val = layer.get_value(row, col);
                if (val >= 0.0f)
                {
                    layer.set_value(row, col, val / maxInfluence);
                }
            }
        }
    }
}

void normalize_dual_occupancy(MapLayer<float> &layer)
{
    /*
        Similar to the solo version, but you need to track greatest positive value AND 
        the least (furthest from 0) negative value.

        For every cell in the given layer, if the value is currently positive divide it by the
        greatest positive value, or if the value is negative divide it by -1.0 * the least negative value
        (so that it remains a negative number).  This will keep the values in the range of [-1, 1].
    */

    // WRITE YOUR CODE HERE

    float maxPosInfluence = -1.0f;
    float maxNegInfluence = 1.0f;
    const int m_width = terrain->get_map_width();

    for (int row = 0; row < m_width; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            const float val = layer.get_value(row, col);
            if (val > 0.0f)
            {
                maxPosInfluence = std::max(maxPosInfluence, val);
            }
            else
            {
                maxNegInfluence = std::min(maxNegInfluence, val);
            }
        }
    }

    maxNegInfluence = -maxNegInfluence;

    for (int row = 0; row < m_width; ++row)
    {
        for (int col = 0; col < m_width; ++col)
        {
            const float val = layer.get_value(row, col);
            if (val > 0.0f && maxPosInfluence > 0.0f)
            {
                const float result = std::min(1.0f, val / maxPosInfluence);
                layer.set_value(row, col, result);
            }
            else if (maxNegInfluence > 0.0f)
            {
                const float result = std::max(-1.0f, val / maxNegInfluence);
                layer.set_value(row, col, result);
            }
        }
    }
}

void enemy_field_of_view(MapLayer<float> &layer, float fovAngle, float closeDistance, float occupancyValue, AStarAgent *enemy)
{
    /*
        First, clear out the old values in the map layer by setting any negative value to 0.
        Then, for every cell in the layer that is within the field of view cone, from the
        enemy agent, mark it with the occupancy value.

        If the tile is close enough to the enemy (less than closeDistance),
        you only check if it's visible to enemy.  Make use of the is_clear_path
        helper function.  Otherwise, you must consider the direction the enemy is facing too.
        This creates a radius around the enemy that the player can be detected within, as well
        as a fov cone.
    */

    // WRITE YOUR CODE HERE
    const int width = terrain->get_map_width();
    const auto pos = enemy->get_position();
    const auto dir = enemy->get_forward_vector();

    const auto gridPos = terrain->get_grid_position(pos);

    const float radsEpsilon = ((fovAngle * 0.5f * PI) / 180.0f) + 0.01f;
    const float dirAngle = atan2(dir.z, dir.x);

    for (int row = 0; row < width; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            if (layer.get_value(row, col) < 0.0f)
            {
                layer.set_value(row, col, 0.0f);
            }

            const float rDelta = static_cast<float>(row - gridPos.row);
            const float cDelta = static_cast<float>(col - gridPos.col);

            const float dist = std::sqrt(rDelta * rDelta + cDelta * cDelta);

            if (dist < closeDistance)
            {
                if (is_clear_path(gridPos.row, gridPos.col, row, col) == true)
                {
                    layer.set_value(row, col, occupancyValue);
                }

                continue;
            }

            auto delta = terrain->get_world_position(row, col) - pos;
            delta.Normalize();
            const float gridAngle = atan2(delta.z, delta.x);

            if (std::abs(dirAngle - gridAngle) <= radsEpsilon)
            {
                if (is_clear_path(gridPos.row, gridPos.col, row, col) == true)
                {
                    layer.set_value(row, col, occupancyValue);
                }
            }
        }
    }
}

bool enemy_find_player(MapLayer<float> &layer, AStarAgent *enemy, Agent *player)
{
    /*
        Check if the player's current tile has a negative value, ie in the fov cone
        or within a detection radius.
    */

    const auto &playerWorldPos = player->get_position();

    const auto playerGridPos = terrain->get_grid_position(playerWorldPos);

    // verify a valid position was returned
    if (terrain->is_valid_grid_position(playerGridPos) == true)
    {
        if (layer.get_value(playerGridPos) < 0.0f)
        {
            return true;
        }
    }

    // player isn't in the detection radius or fov cone, OR somehow off the map
    return false;
}

bool enemy_seek_player(MapLayer<float> &layer, AStarAgent *enemy)
{
    /*
        Attempt to find a cell with a value of 1.0, and then set it as the new target,
        using enemy->path_to.

        If there are multiple cells with 1.0, then pick the cell closest to the enemy.

        Return whether a target cell was found.
    */

    // WRITE YOUR CODE HERE
    const int width = terrain->get_map_width();
    float closestDist = std::numeric_limits<float>().max();
    int closestRow = -1;
    int closestCol = -1;

    const auto &pos = enemy->get_position();
    const auto gridPos = terrain->get_grid_position(pos);

    for (int row = 0; row < width; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            if (layer.get_value(row, col) >= 0.99999999f && terrain->is_wall(row, col) == false)
            {
                const float xDiff = static_cast<float>((col < gridPos.col) ? gridPos.col - col : col - gridPos.col);
                const float yDiff = static_cast<float>((row < gridPos.row) ? gridPos.row - row : row - gridPos.row);

                float dist;

                if (xDiff < yDiff)
                {
                    dist = xDiff * std::sqrt(2.0f) + (yDiff - xDiff);
                }
                else if (yDiff < xDiff)
                {
                    dist = yDiff * std::sqrt(2.0f) + (xDiff - yDiff);
                }
                else
                {
                    dist = xDiff * std::sqrt(2.0f);
                }

                if (dist < closestDist)
                {
                    closestDist = dist;
                    closestRow = row;
                    closestCol = col;
                }
            }
        }
    }

    if (closestRow != -1)
    {
        const auto targetPos = terrain->get_world_position(closestRow, closestCol);
        enemy->path_to(targetPos);
        return true;
    }

    return false;
}


