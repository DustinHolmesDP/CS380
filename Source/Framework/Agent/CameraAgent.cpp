#include <pch.h>
#include "CameraAgent.h"

namespace
{
    const Vec3 defaultEyePosition((Terrain::mapSizeInWorld * 0.4f), Terrain::mapSizeInWorld * 1.45f, Terrain::mapSizeInWorld * 0.5f);
    const float defaultPitch = 1.50098; // 86 degrees
    const float defaultYaw = HALF_PI;
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
    set_yaw(defaultYaw);
    set_pitch(defaultPitch);
}
