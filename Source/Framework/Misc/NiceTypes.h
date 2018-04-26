#pragma once
#include "SimpleMath.h"

#define PI DirectX::XM_PI
#define TWO_PI DirectX::XM_2PI
#define HALF_PI DirectX::XM_PIDIV2
#define QTR_PI DirectX::XM_PIDIV4

using Vec2 = DirectX::SimpleMath::Vector2;
using Vec3 = DirectX::SimpleMath::Vector3;
using Vec4 = DirectX::SimpleMath::Vector4;
using Mat4 = DirectX::SimpleMath::Matrix;
using Quat = DirectX::SimpleMath::Quaternion;
using Rect = DirectX::SimpleMath::Rectangle;
using Color = DirectX::SimpleMath::Color;
namespace Colors = DirectX::Colors;
using Callback = std::function<void(void)>;

extern const Vec3 globalUp;
extern const Vec3 globalRight;
extern const Vec3 globalForward;

template <typename T>
using Setter = std::function<void(const T &)>;

template <typename T>
using Getter = std::function<typename T(void)>;

using TextGetter = std::function<const std::wstring &(void)>;

struct WindowPos
{
    int x;
    int y;

    WindowPos operator+(const WindowPos &rhs) const
    {
        return WindowPos { x + rhs.x, y + rhs.y };
    }

    WindowPos operator-(const WindowPos &rhs) const
    {
        return WindowPos { x - rhs.x, y - rhs.y };
    }
};

struct GridPos
{
    int row;
    int col;

    bool operator==(const GridPos &rhs) const
    {
        return row == rhs.row && col == rhs.col;
    }

    bool operator!=(const GridPos &rhs) const
    {
        return row != rhs.row || col != rhs.col;
    }
};

struct WindowSize
{
    int width;
    int height;
};