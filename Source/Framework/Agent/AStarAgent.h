#pragma once
#include "Agent.h"
#include <list>
#include "misc/PathfindingDetails.hpp"

class AStarAgent final : public Agent
{
public:
    AStarAgent(size_t id);

    virtual void update(float dt) override final;
    static const char *patherTypeName;

    void path_to(const Vec3 &point);

    // all the getters and setters needed to hook directly into ui
    void next_heuristic_type();
    Heuristic get_heuristic_type();
    void set_heuristic_type(const Heuristic &type);
    const std::wstring &get_heuristic_type_text();

    float get_heuristic_weight();
    void set_heuristic_weight(const float &weight);
    const std::wstring &get_heuristic_weight_text();

    void toggle_smoothing();
    bool get_smoothing();
    void set_smoothing(bool state);

    void toggle_rubberbanding();
    bool get_rubberbanding();
    void set_rubberbanding(bool state);

    void toggle_single_step();
    bool get_single_step();
    void set_single_step(bool state);

    void toggle_follow_path();
    bool get_follow_path();
    void set_follow_path(bool state);

    void next_method_type();
    Method get_method_type();
    void set_method_type(Method type);
    const std::wstring &get_method_text();

    void toggle_debug_coloring();
    bool get_debug_coloring();
    void set_debug_coloring(bool state);

    virtual void draw_debug() override final;

    const PathRequest &get_request_data() const;
    void bulk_set_request_settings(const PathRequest::Settings &setting);

private:
    PathRequest request;
    PathRequest buffer;
    bool computingPath;
    bool followPath;
    // we can statically build all other display text but weight
    std::wstring heuristicWeightText;

    void process_request();

    void on_map_change();
    
};