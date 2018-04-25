#pragma once
#include "../Misc/NiceTypes.h"

float lerp(float value0, float value1, float t);

// determines if two 2D lines intersect, each line is defined by two points
bool line_intersect(const Vec2 &line0P0, const Vec2 &line0P1, const Vec2 &line1P0, const Vec2 &line1P1);