#pragma once
#include "Agent.h"

class CameraAgent final : public Agent
{
public:
    CameraAgent();

    Mat4 get_view_matrix() const;
    void set_position_to_default();

    static const char *cameraTypeName;
private:
};