#pragma once
#include "Project.h"
#include "../Student/Project_2/P2_Pathfinding.h"
#include <memory>

class ProjectThree final : public Project
{
public:
    virtual bool initialize() override final;
    virtual bool finalize() override final;
    virtual void shutdown() override final;

    virtual void draw_meshes() override final;
    virtual void draw_sprites() override final;
    virtual void draw_text() override final;
    virtual void draw_debug() override final;

    virtual void update() override final;

    bool implemented_fog_of_war() const;

private:
    AStarAgent *player;
    AStarAgent *enemy;

    std::wstring propagationDecayText;
    std::wstring propagationGrowthText;
    std::wstring analysisFrequencyText;
    std::wstring enemyFOVText;
    std::wstring enemyRadiusText;

    float propagationDecay;
    float propagationGrowth;
    unsigned analysisFrequency;
    unsigned frequencyMod;
    unsigned frequencyOffset;
    float enemyFOV;
    float enemyRadius;

    bool openness;
    bool totalVisibility;
    bool playerVisibility;
    bool search;
    bool propagation;
    bool propagationNormalized;
    bool hideAndSeek;

    unsigned get_analysis_frequency();
    void set_analysis_frequency(const unsigned &val);
    const std::wstring &get_analysis_frequency_text();

    float get_propagation_decay();
    void set_propagation_decay(const float &val);
    const std::wstring &get_propagation_decay_text();

    float get_propagation_growth();
    void set_propagation_growth(const float &val);
    const std::wstring &get_propagation_growth_text();

    float get_enemy_fov();
    void set_enemy_fov(const float &val);
    const std::wstring &get_enemy_fov_text();

    float get_enemy_radius();
    void set_enemy_radius(const float &val);
    const std::wstring &get_enemy_radius_text();


    void build_ui();
    void link_input();
    void on_f1();
    void on_f2();
    void on_mouse_left_click();
    void on_mouse_right_click();

    void toggle_openness();
    void toggle_total_visibility();
    void toggle_player_visibility();
    void toggle_search();
    void toggle_propagation();
    void toggle_propagation_normalized();
    void toggle_hide_and_seek();

    void perform_openness_analysis();
    void perform_total_visibility_analysis();
    void perform_player_visibility_analysis();
    void perform_search_analysis();
    void perform_propagation();
    void perform_propagation_normalized();
    void perform_hide_and_seek();

    bool get_openness_state();
    bool get_total_visibility_state();
    bool get_player_visibility_state();
    bool get_search_state();
    bool get_propagation_state();
    bool get_propagation_normalized_state();
    bool get_hide_and_seek_state();
};