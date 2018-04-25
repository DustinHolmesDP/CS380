#include <pch.h>
#include "Agent.h"
#include "../Rendering/DrawPackage.h"

// TODO: move this temp model stuff to renderer in some capacity, if time permits
std::unique_ptr<DirectX::Model> Agent::model;

Agent::Agent(const char *type, size_t id) : position(0.0f, 0.0f, 0.0f),
    scaling(3.0f, 3.0f, 3.0f), rotation(Mat4::Identity), isDirty(true), color(0.7f, 0.7f, 0.7f), type(type), id(id), movementSpeed(500.0f)
{}

#pragma region Getters

const Vec3 &Agent::get_position() const
{
    return position;
}

const Vec3 &Agent::get_scaling() const
{
    return scaling;
}

Vec3 Agent::get_forward_vector() const
{
    //return rotation.Forward();
    return Vec3(-rotation(0, 2), -rotation(1, 2), -rotation(2, 2));
}

Vec3 Agent::get_right_vector() const
{
    //return rotation.Right();
    return Vec3(rotation(0, 0), rotation(1, 0), rotation(2, 0));
}

Vec3 Agent::get_up_vector() const
{
    //return rotation.Up();
    return Vec3(rotation(0, 1), rotation(1, 1), rotation(2, 1));
}

const Mat4 &Agent::get_rotation() const
{
    return rotation;
}

const Vec3 &Agent::get_color() const
{
    return color;
}

const char *Agent::get_type() const
{
    return type;
}

const size_t &Agent::get_id() const
{
    return id;
}

float Agent::get_movement_speed() const
{
    return movementSpeed;
}

const Mat4 &Agent::get_local_to_world()
{
    if (isDirty == true)
    {
        build_transformation();
        isDirty = false;
    }

    return localToWorld;
}

#pragma endregion

#pragma region Setters
void Agent::set_position(const Vec3 &pos)
{
    position = pos;
    isDirty = true;
}

void Agent::set_scaling(const Vec3 &scale)
{
    scaling = scale;
    isDirty = true;
}

void Agent::set_scaling(float scalar)
{
    scaling = Vec3(scalar, scalar, scalar);
    isDirty = true;
}

void Agent::look_in_direction(const Vec3 &dir)
{
    rotation = Mat4::CreateLookAt(position, position + dir, Vec3::Up);
    //auto quat = Quat::CreateFromRotationMatrix(rotation);
    //rotation = Mat4::CreateFromQuaternion(quat);
    isDirty = true;
}

void Agent::look_at_point(const Vec3 &point)
{
    rotation = Mat4::CreateLookAt(position, point, Vec3::Up);
    isDirty = true;
}

void Agent::set_rotation(const Mat4 &rot)
{
    rotation = rot;
    isDirty = true;
}

void Agent::set_color(const Vec3 &newColor)
{
    color = newColor;
}

void Agent::set_movement_speed(float speed)
{
    movementSpeed = speed;
}
#pragma endregion

void Agent::update(float dt)
{}

void Agent::draw_mesh()
{
    model->UpdateEffects([this](DirectX::IEffect *effect)
    {
        auto basic = dynamic_cast<DirectX::BasicEffect *>(effect);
        basic->SetDiffuseColor(color);
    });
    
    auto &resources = renderer->get_resources();
    model->Draw(resources.get_context(), *resources.get_states(), get_local_to_world(), renderer->get_view_matrix(), renderer->get_projection_matrix());
}

void Agent::draw_debug()
{
    // no op
}

bool Agent::acquire_rendering_resources()
{
    std::cout << "Loading Agent model..." << std::endl;
    auto device = renderer->get_resources().get_device();
    auto factory = renderer->get_resources().get_effect_factory();
    try
    {
        model = DirectX::Model::CreateFromSDKMESH(device, L"Assets\\mannequin_rig.sdkmesh", *factory);
    }
    catch (const std::exception &err)
    {
        std::cout << "Unable to load Agent model: " << err.what() << std::endl;
        return false;
    }

    model->UpdateEffects([](DirectX::IEffect *effect)
    {
        auto basic = dynamic_cast<DirectX::BasicEffect*>(effect);
        if (basic)
        {
            basic->SetLightingEnabled(true);
            basic->SetPerPixelLighting(true);
            basic->SetLightEnabled(0, true);
            basic->SetLightDiffuseColor(0, DirectX::Colors::White);

            basic->SetLightEnabled(1, false);
            basic->SetLightEnabled(2, false);
        }
    });

    return true;
}

void Agent::release_rendering_resources()
{
    model.reset();
}

void Agent::update_model(const Mat4 &view, const Mat4 &proj)
{
    model->UpdateEffects([view, proj](DirectX::IEffect *effect)
    {
        auto basic = dynamic_cast<DirectX::BasicEffect *>(effect);
        basic->SetView(view);
        basic->SetProjection(proj);
    });
}

void Agent::build_transformation()
{
    const auto translationMatrix = Mat4::CreateTranslation(position);
    const auto scalingMatrix = Mat4::CreateScale(scaling * globalScalar);
    //localToWorld = scalingMatrix;
    //localToWorld *= rotation;
    //localToWorld *= translationMatrix;
    
    //localToWorld = translationMatrix * rotation * scalingMatrix;
    localToWorld = scalingMatrix * rotation * translationMatrix;
}
