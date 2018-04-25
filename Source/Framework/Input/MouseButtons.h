#pragma once

enum class MouseButtons
{
    LEFT,
    RIGHT,
    MIDDLE,
    BACK,
    FORWARD,

    NUM_ENTRIES
};

MouseButtons convert_mouse_from_windows(WPARAM wParam);