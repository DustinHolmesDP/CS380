#include <pch.h>
#include "ProjectThree.h"
#include "Misc/TimeTracker.h"
#include "Terrain/TerrainAnalysis.h"
#include "Agent/AStarAgent.h"
#include <sstream>

#include "UI/Elements/Buttons/UIButton.h"
#include "UI/Elements/Sliders/UISlider.h"
#include "UI/Elements/Text/UIValueTextField.h"
#include "UI/Elements/Buttons/UIDynamicButton.h"
#include "UI/Elements/Buttons/UIToggleButton.h"

TimeTracker<std::chrono::microseconds> analysisTimer;
bool enemyFoundPlayer = false;

bool ProjectThree::initialize()
{
    std::cout << "Initializing Project Three..." << std::endl;

    // create all the systems that project one requires
    terrain = std::make_unique<Terrain>();
    agents = std::make_unique<AgentOrganizer>();
    ui = std::make_unique<UICoordinator>();
    pather = std::make_unique<AStarPather>();

    set_analysis_frequency(3);

    return terrain->initialize() &&
        agents->initialize() &&
        ui->initialize() &&
        pather->initialize();
}

bool ProjectThree::finalize()
{
    player = agents->create_pathing_agent();
    player->set_debug_coloring(false);
    player->set_single_step(false);
    player->set_color(Vec3(0.0f, 0.0f, 0.8f));

    enemy = agents->create_pathing_agent();
    enemy->set_debug_coloring(false);
    enemy->set_single_step(false);
    enemy->set_color(Vec3(0.8f, 0.0f, 0.0f));

    set_propagation_growth(0.15f);
    set_propagation_decay(0.05f);

    set_enemy_fov(45.0f);
    set_enemy_radius(5.0f);

    build_ui();

    terrain->goto_map(0);
    terrain->pathLayer.set_enabled(true);

    // set up the time tracker to listen for analysis messages
    Callback timerResetCB = std::bind(&decltype(analysisTimer)::reset, &analysisTimer);
    Messenger::listen_for_message(Messages::ANALYSIS_BEGIN, timerResetCB);

    Callback timerStartCB = std::bind(&decltype(analysisTimer)::start, &analysisTimer);
    Messenger::listen_for_message(Messages::ANALYSIS_TICK_START, timerStartCB);

    Callback timerStopCB = std::bind(&decltype(analysisTimer)::stop, &analysisTimer);
    Messenger::listen_for_message(Messages::ANALYSIS_TICK_FINISH, timerStopCB);

    link_input();

    return true;
}

void ProjectThree::shutdown()
{
    std::cout << "Shutting Down Project Three..." << std::endl;

    ui->shutdown();
    ui.reset();

    agents->shutdown();
    agents.reset();

    terrain->shutdown();
    terrain.reset();
}

void ProjectThree::draw_meshes()
{
    terrain->draw();
    agents->draw();
}

void ProjectThree::draw_sprites()
{
    ui->draw_sprites();
}

void ProjectThree::draw_text()
{
    ui->draw_text();
}

void ProjectThree::draw_debug()
{
    terrain->draw_debug();
    agents->draw_debug();
    ui->draw_debug();
}

void ProjectThree::update()
{
    // have the ui coordinator determine its state
    ui->update();

    // have the input system update its current state and send out notifications
    InputHandler::update();

    agents->update(deltaTime);

    // use a frame offset to stagger the execution of various analysis functions
    auto frameDelay = engine->get_timer().GetFrameCount() % frequencyMod;

    if (playerVisibility == true && frameDelay == 0)
    {
        perform_player_visibility_analysis();
    }

    if (search == true)
    {
        terrain->agentVisionLayer.decay(0.995f);

        if (frameDelay == frequencyOffset)
        {
            perform_search_analysis();
        }
    }

    if (propagation == true && frameDelay == (frequencyOffset * 2))
    {
        perform_propagation();
    }
    else if (propagationNormalized == true && frameDelay == (frequencyOffset * 2))
    {
        perform_propagation_normalized();
    }

    if (hideAndSeek == true)
    {
        if (frameDelay == (frequencyOffset * 3))
        {
            propagate_solo_occupancy(terrain->seekLayer, propagationDecay, propagationGrowth);
            normalize_solo_occupancy(terrain->seekLayer);
        }

        perform_hide_and_seek();
    }
}

unsigned ProjectThree::get_analysis_frequency()
{
    return analysisFrequency;
}

void ProjectThree::set_analysis_frequency(const unsigned &val)
{
    analysisFrequency = val;
    analysisFrequencyText = std::to_wstring(analysisFrequency);

    frequencyMod = 60 / analysisFrequency;
    frequencyOffset = frequencyMod / 4;
}

const std::wstring &ProjectThree::get_analysis_frequency_text()
{
    return analysisFrequencyText;
}

float ProjectThree::get_propagation_decay()
{
    return propagationDecay;
}

void ProjectThree::set_propagation_decay(const float &val)
{
    propagationDecay = val;
    std::wstringstream stream;
    stream.precision(3);
    stream << propagationDecay;
    propagationDecayText = stream.str();
}

const std::wstring &ProjectThree::get_propagation_decay_text()
{
    return propagationDecayText;
}

float ProjectThree::get_propagation_growth()
{
    return propagationGrowth;
}

void ProjectThree::set_propagation_growth(const float &val)
{
    propagationGrowth = val;
    std::wstringstream stream;
    stream.precision(3);
    stream << propagationGrowth;
    propagationGrowthText = stream.str();
}

const std::wstring &ProjectThree::get_propagation_growth_text()
{
    return propagationGrowthText;
}

float ProjectThree::get_enemy_fov()
{
    return enemyFOV;
}

void ProjectThree::set_enemy_fov(const float &val)
{
    enemyFOV = val;
    std::wstringstream stream;
    stream.precision(3);
    stream << enemyFOV;
    enemyFOVText = stream.str();
}

const std::wstring &ProjectThree::get_enemy_fov_text()
{
    return enemyFOVText;
}

float ProjectThree::get_enemy_radius()
{
    return enemyRadius;
}

void ProjectThree::set_enemy_radius(const float &val)
{
    enemyRadius = val;
    std::wstringstream stream;
    stream.precision(3);
    stream << enemyRadius;
    enemyRadiusText = stream.str();
}

const std::wstring &ProjectThree::get_enemy_radius_text()
{
    return enemyRadiusText;
}

void ProjectThree::build_ui()
{
    // add a text field at the top for the project
    auto projectText = ui->create_text_field(UIAnchor::TOP, 0, 32, UIAnchor::CENTER, L"Project Three");

    // create the first button in the top right,
    // 90 is half button width + 16 (padding from side of screen) + 10 (padding so sliders aren't directly against the edge)
    // 32 is half button height + 16 (padding from top of screen)
    Callback nextMapCB = std::bind(&Terrain::goto_next_map, terrain.get());
    auto mapButton = ui->create_button(UIAnchor::TOP_RIGHT, -90, 32, nextMapCB, L"Next Map");

    // setup the analysis buttons
    Callback opennessCB = std::bind(&ProjectThree::toggle_openness, this);
    Getter<bool> opennessGet = std::bind(&ProjectThree::get_openness_state, this);
    auto opennessButton = ui->create_toggle_button(UIAnchor::BOTTOM, mapButton,
        10, opennessCB, L"Openness", opennessGet);

    Callback totalVisCB = std::bind(&ProjectThree::toggle_total_visibility, this);
    Getter<bool> totalVisGet = std::bind(&ProjectThree::get_total_visibility_state, this);
    auto totalVisButton = ui->create_toggle_button(UIAnchor::BOTTOM, opennessButton,
        10, totalVisCB, L"Visibility", totalVisGet);

    Callback playerVisCB = std::bind(&ProjectThree::toggle_player_visibility, this);
    Getter<bool> playerVisGet = std::bind(&ProjectThree::get_player_visibility_state, this);
    auto playerVisButton = ui->create_toggle_button(UIAnchor::BOTTOM, totalVisButton,
        10, playerVisCB, L"Visible To Agent", playerVisGet);

    Callback searchCB = std::bind(&ProjectThree::toggle_search, this);
    Getter<bool> searchGet = std::bind(&ProjectThree::get_search_state, this);
    auto searchButton = ui->create_toggle_button(UIAnchor::BOTTOM, playerVisButton,
        10, searchCB, L"Search", searchGet);

    Callback propagationCB = std::bind(&ProjectThree::toggle_propagation, this);
    Getter<bool> propagationGet = std::bind(&ProjectThree::get_propagation_state, this);
    auto propagationButton = ui->create_toggle_button(UIAnchor::BOTTOM, searchButton,
        10, propagationCB, L"Propagation", propagationGet);

    Callback normalizedCB = std::bind(&ProjectThree::toggle_propagation_normalized, this);
    Getter<bool> normalizedGet = std::bind(&ProjectThree::get_propagation_normalized_state, this);
    auto normalizedButton = ui->create_toggle_button(UIAnchor::BOTTOM, propagationButton,
        10, normalizedCB, L"Normalized Prop.", normalizedGet);

    Callback hideCB = std::bind(&ProjectThree::toggle_hide_and_seek, this);
    Getter<bool> hideGet = std::bind(&ProjectThree::get_hide_and_seek_state, this);
    auto hideButton = ui->create_toggle_button(UIAnchor::BOTTOM, normalizedButton,
        10, hideCB, L"Hide and Seek", hideGet);

    



    // add some text on the left side for displaying fps
    TextGetter fpsGetter = std::bind(&Engine::get_fps_text, engine.get());
    auto fpsText = ui->create_value_text_field(UIAnchor::TOP_LEFT, 90, 32, L"FPS:", fpsGetter);

    // and the pathing time
    TextGetter timeGetter = std::bind(&decltype(analysisTimer)::get_text, &analysisTimer);
    auto analysisTimeText = ui->create_value_text_field(UIAnchor::BOTTOM, fpsText, 10, L"Time:", timeGetter);

    // a slider to control how often analysis happens
    Getter<unsigned> frequencyGet = std::bind(&ProjectThree::get_analysis_frequency, this);
    Setter<unsigned> frequencySet = std::bind(&ProjectThree::set_analysis_frequency, this, std::placeholders::_1);
    TextGetter frequencyText = std::bind(&ProjectThree::get_analysis_frequency_text, this);
    auto frequencySlider = ui->create_slider<unsigned>(UIAnchor::BOTTOM, analysisTimeText, 10,
        1, 20, frequencyGet, frequencySet, frequencyText, L"Ticks Per Sec:");

    Getter<float> decayGet = std::bind(&ProjectThree::get_propagation_decay, this);
    Setter<float> decaySet = std::bind(&ProjectThree::set_propagation_decay, this, std::placeholders::_1);
    TextGetter decayText = std::bind(&ProjectThree::get_propagation_decay_text, this);
    auto decaySlider = ui->create_slider<float>(UIAnchor::BOTTOM, frequencySlider,
        10, 0.0f, 0.1f, decayGet, decaySet, decayText, L"Decay");

    Getter<float> growthGet = std::bind(&ProjectThree::get_propagation_growth, this);
    Setter<float> growthSet = std::bind(&ProjectThree::set_propagation_growth, this, std::placeholders::_1);
    TextGetter growthText = std::bind(&ProjectThree::get_propagation_growth_text, this);
    auto growthSlider = ui->create_slider<float>(UIAnchor::BOTTOM, decaySlider,
        10, 0.0f, 1.0f, growthGet, growthSet, growthText, L"Growth");

    Getter<float> fovGet = std::bind(&ProjectThree::get_enemy_fov, this);
    Setter<float> fovSet = std::bind(&ProjectThree::set_enemy_fov, this, std::placeholders::_1);
    TextGetter fovText = std::bind(&ProjectThree::get_enemy_fov_text, this);
    auto fovSlider = ui->create_slider<float>(UIAnchor::BOTTOM, growthSlider,
        10, 45.0f, 180.0f, fovGet, fovSet, fovText, L"Enemy FOV");

    Getter<float> radiusGet = std::bind(&ProjectThree::get_enemy_radius, this);
    Setter<float> radiusSet = std::bind(&ProjectThree::set_enemy_radius, this, std::placeholders::_1);
    TextGetter radiusText = std::bind(&ProjectThree::get_enemy_radius_text, this);
    auto radiusSlider = ui->create_slider<float>(UIAnchor::BOTTOM, fovSlider,
        10, 3.0f, 10.0f, radiusGet, radiusSet, radiusText, L"Enemy Dist.");
}

void ProjectThree::link_input()
{
    Callback f1CB = std::bind(&ProjectThree::on_f1, this);
    InputHandler::notify_when_key_pressed(KBKeys::F1, f1CB);

    Callback f2CB = std::bind(&ProjectThree::on_f2, this);
    InputHandler::notify_when_key_pressed(KBKeys::F2, f2CB);

    Callback lmbCB = std::bind(&ProjectThree::on_mouse_left_click, this);
    InputHandler::notify_when_mouse_pressed(MouseButtons::LEFT, lmbCB);

    Callback rmbCB = std::bind(&ProjectThree::on_mouse_right_click, this);
    InputHandler::notify_when_mouse_pressed(MouseButtons::RIGHT, rmbCB);
}

void ProjectThree::on_f2()
{
    engine->change_projects(Project::Type::TWO);
}

void ProjectThree::on_mouse_left_click()
{
    const auto &mousePos = InputHandler::get_mouse_position();

    // convert the mouse position to a point on the terrain plane
    const auto worldPos = renderer->screen_to_world(mousePos.x, mousePos.y, terrain->get_terrain_plane());

    // verify a valid point was determine
    if (worldPos.second == true)
    {
        // convert the world position to a terraing grid position
        const auto gridPos = terrain->get_grid_position(worldPos.first);

        // verify that the grid position is valid
        if (terrain->is_valid_grid_position(gridPos) == true && terrain->is_wall(gridPos) == false)
        {
            if (propagation || propagationNormalized)
            {
                terrain->occupancyLayer.set_value(gridPos, 1.0f);
            }
            else
            {
                // have the agent path to the position
                player->path_to(worldPos.first);
            }
        }
    }
}

void ProjectThree::on_mouse_right_click()
{
    const auto &mousePos = InputHandler::get_mouse_position();

    // convert the mouse position to a point on the terrain plane
    const auto worldPos = renderer->screen_to_world(mousePos.x, mousePos.y, terrain->get_terrain_plane());

    // verify a valid point was determine
    if (worldPos.second == true)
    {
        // convert the world position to a terraing grid position
        const auto gridPos = terrain->get_grid_position(worldPos.first);

        // verify that the grid position is valid
        if (terrain->is_valid_grid_position(gridPos) == true && terrain->is_wall(gridPos) == false)
        {
            if (propagation || propagationNormalized)
            {
                terrain->occupancyLayer.set_value(gridPos, -1.0f);
            }
            else
            {
                // have the enemy path to the position
                enemy->path_to(worldPos.first);
            }
        }
    }
}

void ProjectThree::toggle_openness()
{
    openness = !openness;

    if (openness == true)
    {
        
        perform_openness_analysis();
    }

    terrain->opennessLayer.set_enabled(openness);
}

void ProjectThree::toggle_total_visibility()
{
    totalVisibility = !totalVisibility;

    if (totalVisibility == true)
    {
        perform_total_visibility_analysis();
    }

    terrain->totalVisibilityLayer.set_enabled(totalVisibility);
}

void ProjectThree::toggle_player_visibility()
{
    playerVisibility = !playerVisibility;

    terrain->cellVisibilityLayer.set_enabled(playerVisibility);
}

void ProjectThree::toggle_search()
{
    search = !search;

    terrain->agentVisionLayer.set_enabled(search);
}

void ProjectThree::toggle_propagation()
{
    propagation = !propagation;

    // propagation and normalized utilize the same layer
    if (propagation == true)
    {
        propagationNormalized = false;
        hideAndSeek = false;
    }

    terrain->occupancyLayer.set_enabled(propagation);
}

void ProjectThree::toggle_propagation_normalized()
{
    propagationNormalized = !propagationNormalized;

    // propagation and normalized utilize the same layer
    if (propagationNormalized == true)
    {
        propagation = false;
    }

    terrain->occupancyLayer.set_enabled(propagationNormalized);
}

void ProjectThree::toggle_hide_and_seek()
{
    hideAndSeek = !hideAndSeek;

    if (hideAndSeek == true)
    {
        terrain->seekLayer.set_all_values(0.0f);
    }

    terrain->seekLayer.set_enabled(hideAndSeek);
}

void ProjectThree::perform_openness_analysis()
{
    Messenger::send_message(Messages::ANALYSIS_BEGIN);
    Messenger::send_message(Messages::ANALYSIS_TICK_START);

    analyze_openness(terrain->opennessLayer);

    Messenger::send_message(Messages::ANALYSIS_TICK_FINISH);
    Messenger::send_message(Messages::ANALYSIS_END);
}

void ProjectThree::perform_total_visibility_analysis()
{
    Messenger::send_message(Messages::ANALYSIS_BEGIN);
    Messenger::send_message(Messages::ANALYSIS_TICK_START);

    analyze_visibility(terrain->totalVisibilityLayer);

    Messenger::send_message(Messages::ANALYSIS_TICK_FINISH);
    Messenger::send_message(Messages::ANALYSIS_END);
}

void ProjectThree::perform_player_visibility_analysis()
{
    const auto pos = terrain->get_grid_position(player->get_position());

    if (terrain->is_valid_grid_position(pos) == true)
    {
        Messenger::send_message(Messages::ANALYSIS_BEGIN);
        Messenger::send_message(Messages::ANALYSIS_TICK_START);

        analyze_visble_to_cell(terrain->cellVisibilityLayer, pos.row, pos.col);

        Messenger::send_message(Messages::ANALYSIS_TICK_FINISH);
        Messenger::send_message(Messages::ANALYSIS_END);
    }
}

void ProjectThree::perform_search_analysis()
{
    Messenger::send_message(Messages::ANALYSIS_BEGIN);
    Messenger::send_message(Messages::ANALYSIS_TICK_START);

    analyze_agent_vision(terrain->agentVisionLayer, player);

    Messenger::send_message(Messages::ANALYSIS_TICK_FINISH);
    Messenger::send_message(Messages::ANALYSIS_END);
}

void ProjectThree::perform_propagation()
{
    Messenger::send_message(Messages::ANALYSIS_BEGIN);
    Messenger::send_message(Messages::ANALYSIS_TICK_START);

    propagate_dual_occupancy(terrain->occupancyLayer, propagationDecay, propagationGrowth);

    Messenger::send_message(Messages::ANALYSIS_TICK_FINISH);
    Messenger::send_message(Messages::ANALYSIS_END);
}

void ProjectThree::perform_propagation_normalized()
{
    Messenger::send_message(Messages::ANALYSIS_BEGIN);
    Messenger::send_message(Messages::ANALYSIS_TICK_START);

    propagate_dual_occupancy(terrain->occupancyLayer, propagationDecay, propagationGrowth);
    normalize_dual_occupancy(terrain->occupancyLayer);

    Messenger::send_message(Messages::ANALYSIS_TICK_FINISH);
    Messenger::send_message(Messages::ANALYSIS_END);
}

void ProjectThree::perform_hide_and_seek()
{
    Messenger::send_message(Messages::ANALYSIS_BEGIN);
    Messenger::send_message(Messages::ANALYSIS_TICK_START);

    enemy_field_of_view(terrain->seekLayer, enemyFOV, enemyRadius, -0.5f, enemy);

    // see if the player is within the detection area
    if (enemy_find_player(terrain->seekLayer, enemy, player) == false)
    {
        if (enemyFoundPlayer == false)
        {
            // see if the enemy doesn't currently have a goal
            if (enemy->get_request_data().path.size() == 0)
            {
                // see if there is a position it makes sense to look for the player
                if (enemy_seek_player(terrain->seekLayer, enemy) == false)
                {
                    // just pick a random nearby point
                    const auto &worldPos = enemy->get_position();
                    const auto gridPos = terrain->get_grid_position(worldPos);

                    GridPos target;

                    const int minRow = std::max(gridPos.row - 5, 0);
                    const int maxRow = std::min(gridPos.row + 5, terrain->get_map_height());
                    const int minCol = std::max(gridPos.col - 5, 0);
                    const int maxCol = std::min(gridPos.col + 5, terrain->get_map_width());

                    while (true)
                    {
                        target.row = RNG::range(minRow, maxRow);
                        target.col = RNG::range(minCol, maxCol);

                        if (terrain->is_valid_grid_position(target) == true && terrain->is_wall(target) == false)
                        {
                            break;
                        }
                    }

                    const auto worldTarget = terrain->get_world_position(target);
                    enemy->path_to(worldTarget);
                }
            }
        }
        else
        {
            enemyFoundPlayer = false;
        }
    }
    else
    {
        const auto &playerWorld = player->get_position();
        const auto playerGrid = terrain->get_grid_position(playerWorld);
        
        auto &layer = terrain->seekLayer;

        if (enemyFoundPlayer == false)
        {
            const int width = terrain->get_map_width();
            const int height = terrain->get_map_height();
            for (int h = 0; h < height; ++h)
            {
                for (int w = 0; w < width; ++w)
                {
                    if (layer.get_value(h, w) > 0.0f)
                    {
                        layer.set_value(h, w, 0.0f);
                    }
                }
            }

            layer.set_value(playerGrid, 1.0f);
            for (int i = 0; i < 3; ++i)
            {
                propagate_solo_occupancy(layer, propagationDecay, propagationGrowth);
            }

            enemyFoundPlayer = true;
            enemy->path_to(playerWorld);
            Messenger::send_message(Messages::PATH_REQUEST_END);
        }
        else if (enemy->get_request_data().path.size() == 0)
        {
            enemyFoundPlayer = false;
        }

        
    }

    Messenger::send_message(Messages::ANALYSIS_TICK_FINISH);
    Messenger::send_message(Messages::ANALYSIS_END);
}

bool ProjectThree::get_openness_state()
{
    return openness;
}

bool ProjectThree::get_total_visibility_state()
{
    return totalVisibility;
}

bool ProjectThree::get_player_visibility_state()
{
    return playerVisibility;
}

bool ProjectThree::get_search_state()
{
    return search;
}

bool ProjectThree::get_propagation_state()
{
    return propagation;
}

bool ProjectThree::get_propagation_normalized_state()
{
    return propagationNormalized;
}

bool ProjectThree::get_hide_and_seek_state()
{
    return hideAndSeek;
}

void ProjectThree::on_f1()
{
    engine->change_projects(Project::Type::ONE);
}

