#pragma once
#include "../Misc/NiceTypes.h"

// forward declarations
class AgentOrganizer;

class Agent
{
    friend class AgentOrganizer;
public:
    Agent(const char *type, size_t id);

#pragma region Getters
    const Vec3 &get_position() const;
    const Vec3 &get_scaling() const;

    Vec3 get_forward_vector() const;
    Vec3 get_right_vector() const;
    Vec3 get_up_vector() const;
    const Mat4 &get_rotation() const;

    const Vec3 &get_color() const;
    
    const Mat4 &get_local_to_world();

    const char *get_type() const;
    const size_t &get_id() const;

    float get_movement_speed() const;
#pragma endregion

#pragma region Setters
    void set_position(const Vec3 &pos);
    void set_scaling(const Vec3 &scale);
    void set_scaling(float scalar);

    void look_in_direction(const Vec3 &dir);
    void look_at_point(const Vec3 &point);
    void set_rotation(const Mat4 &rot);

    void set_color(const Vec3 &newColor);
    
    void set_movement_speed(float speed);
#pragma endregion

    virtual void update(float dt);

private:
    Vec3 position;
    Vec3 scaling;
    Mat4 rotation;

    Mat4 localToWorld;
    bool isDirty;

    Vec3 color;
    
    const char *type;
    const size_t id;

    float movementSpeed;

    static std::unique_ptr<DirectX::Model> model;

    void build_transformation();

    void draw_mesh();
    virtual void draw_debug();
    static bool acquire_rendering_resources();
    static void release_rendering_resources();
    static void update_model(const Mat4 &view, const Mat4 &proj);
};