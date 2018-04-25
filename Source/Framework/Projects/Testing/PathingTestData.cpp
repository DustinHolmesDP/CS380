#include <pch.h>
#include "PathingTestData.h"
#include "Agent/AStarAgent.h"

PathingTestData::PathingTestData() :
    map(-1), start { -1, -1 }, goal { -1, -1 }, distance(-1), message(nullptr),
    hasSolution(false), requiresVisualConfirmation(false)
{}

PathingTestData::PathingTestData(const PathingTestData &other) :
    map(other.map), start(other.start), goal(other.goal), distance(other.distance), message(other.message), hasSolution(other.hasSolution),
    requiresVisualConfirmation(other.requiresVisualConfirmation)
{}

PathingTestData &PathingTestData::operator=(const PathingTestData &rhs)
{
    map = rhs.map;
    start = rhs.start;
    goal = rhs.goal;
    distance = 0;
    message = nullptr;
    hasSolution = rhs.hasSolution;
    requiresVisualConfirmation = rhs.requiresVisualConfirmation;

    return *this;
}

PathingTestData::Outcome PathingTestData::operator()(AStarAgent *agent)
{
    terrain->goto_map(static_cast<unsigned>(map));

    const auto startPos = terrain->get_world_position(start);

    // set the parameters of this test
    agent->set_position(startPos);
    
    const auto goalPos = terrain->get_world_position(goal);

    agent->path_to(goalPos);

    const auto &pathData = agent->get_request_data();

    Outcome outcome = Outcome::INVALID;

    // check if we got a path and we were expecting one
    const bool pathFound = pathData.path.size() > 0;

    if (pathFound == true && hasSolution == true)
    {
        // verify the start and goal positions are correct;
        const auto startPos = terrain->get_grid_position(pathData.path.front());
        const auto goalPos = terrain->get_grid_position(pathData.path.back());

        if (startPos != start)
        {
            message = "Failed: Start position of the path is incorrect";
            outcome = Outcome::FAILED;
        }
        else if (goalPos != goal)
        {
            message = "Failed: Goal position of the path is incorrect";
            outcome = Outcome::FAILED;
        }
        else
        {
            // attempt to calculate the path's distance, which may discover a fault
            if (calculate_distance(pathData.path) == true)
            {
                outcome = Outcome::VALID;
            }
            else
            {
                outcome = Outcome::FAILED;
            }
        }
    }
    else if (pathFound == true && hasSolution == false)
    {
        message = "Failed: A path was found for an impossible goal";
        outcome = Outcome::FAILED;
    }
    else if (pathFound == false && hasSolution == true)
    {
        message = "Failed: A path was not found for a valid goal";
        outcome = Outcome::FAILED;
    }
    else if (pathFound == false && hasSolution == false)
    {
        outcome = Outcome::VALID;
    }
    
    return outcome;
}

PathingTestData::Outcome PathingTestData::operator==(const PathingTestData &rhs)
{
    Outcome outcome = Outcome::INVALID;

    if (requiresVisualConfirmation == false)
    {
        if (hasSolution == true)
        {
            const bool equivalentPath = distance == rhs.distance;

            if (equivalentPath == true)
            {
                outcome = Outcome::PASSED;
                message = "Passed: Path is equivalent to expected";
            }
            else
            {
                outcome = Outcome::FAILED;
                message = "Failed: Path is not optimal for heuristic and weight";
            }
        }
        else if (message == nullptr)
        {
            outcome = Outcome::PASSED;
            message = "Passed: No path was found to impossible goal";
        }
    }
    else
    {
        outcome = Outcome::SCREEN;
    }

    return outcome;
}

const char *PathingTestData::get_message_text() const
{
    return message;
}

void PathingTestData::bootstrap(AStarAgent *agent, const GridPos &st, const GridPos &gl, int m)
{
    map = m;
    start = st;
    goal = gl;
    message = nullptr;

    const auto startPos = terrain->get_world_position(start);

    // set the parameters of this test
    agent->set_position(startPos);

    const auto goalPos = terrain->get_world_position(goal);

    agent->path_to(goalPos);

    const auto &pathData = agent->get_request_data();

    if (pathData.path.size() == 1)
    {
        hasSolution = false;
        distance = -1;
    }
    else
    {
        hasSolution = true;
        calculate_distance(pathData.path);
    }
}

bool PathingTestData::calculate_distance(const WaypointList &path)
{
    distance = 0;
    auto p0 = path.begin();
    auto p1 = p0;
    ++p1;

    while (p1 != path.end())
    {
        const auto g0 = terrain->get_grid_position(*p0);
        const auto g1 = terrain->get_grid_position(*p1);

        if (calculate_distance(g0, g1) == false)
        {
            return false;
        }

        p0 = p1;
        ++p1;
    }

    return true;
}

bool PathingTestData::calculate_distance(const GridPos &p0, const GridPos &p1)
{
    bool outcome = true;

    const int rowDiff = std::abs(p0.row - p1.row);
    const int colDiff = std::abs(p0.col - p1.col);

    // if greater than 1 the cells aren't neighbors
    if (rowDiff > 1 || colDiff > 1)
    {
        outcome = false;
        message = "Failed: Two consecutive points in the path are not neighbors";
    }
    // or if the two points are the same
    else if (p0 == p1)
    {
        outcome = false;
        message = "Failed: Two consecutive points in the path are the same";
    }
    else
    {
        distance += (rowDiff * rowDiff) + (colDiff * colDiff);
    }

    return outcome;
}
