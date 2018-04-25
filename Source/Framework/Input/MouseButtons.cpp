#include <pch.h>
#include "MouseButtons.h"

MouseButtons convert_mouse_from_windows(WPARAM wParam)
{
    switch (wParam)
    {
    case 0x01:
        return MouseButtons::LEFT;
    case 0x02:
        return MouseButtons::RIGHT;
    case 0x10:
        return MouseButtons::MIDDLE;
    case 0x20:
        return MouseButtons::BACK;
    case 0x40:
        return MouseButtons::FORWARD;
    default:
        return MouseButtons::NUM_ENTRIES;
    }
}