#pragma once
#include "AStarAgent.h"

class EnemyAgent final : public AStarAgent
{
public:
    EnemyAgent(size_t id);
    virtual void path_to(const Vec3 &point);

    void logic_tick();

    float get_fov();
    void set_fov(const float &val);
    const std::wstring &get_fov_text();

    float get_radius();
    void set_radius(const float &val);
    const std::wstring &get_radius_text();

    void set_player(AStarAgent *player);

private:
    enum class State
    {
        IDLE,
        CHASE,
        SEEK,
        PATROL
    } state;

    float fov;
    float radius;
    std::wstring fovText;
    std::wstring radiusText;
    AStarAgent *player;
    GridPos playerPrevious;

    void choose_random_goal();
};
