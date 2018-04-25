#include <pch.h>
#include "CameraAgent.h"

namespace
{
    //const Vec3 defaultEyePosition((Terrain::mapSizeInWorld * 0.4f), Terrain::mapSizeInWorld * 1.4f, Terrain::mapSizeInWorld * 0.5f);
    //const Vec3 defaultLookPosition(Terrain::mapSizeInWorld * 0.5f, Terrain::mapSizeInWorld * -0.1f, Terrain::mapSizeInWorld * 0.5f);

    const Vec3 defaultEyePosition((Terrain::mapSizeInWorld * 0.4f), Terrain::mapSizeInWorld * 1.4f, Terrain::mapSizeInWorld * 0.5f);
    const Vec3 defaultLookPosition(Terrain::mapSizeInWorld * 0.5f, Terrain::mapSizeInWorld * -0.1f, Terrain::mapSizeInWorld * 0.5f);
}

const char *CameraAgent::cameraTypeName = "Camera";

CameraAgent::CameraAgent() : Agent(cameraTypeName, 0)
{
    set_position_to_default();
}

Mat4 CameraAgent::get_view_matrix() const
{
    const auto &eye = get_position();
    const auto forward = get_forward_vector();
    const auto point = eye + forward;

    return Mat4::CreateLookAt(eye, point, Vec3::Up);
}

void CameraAgent::set_position_to_default()
{
    set_position(defaultEyePosition);
    look_at_point(defaultLookPosition);
}
