#include <pch.h>
#include "UIDynamicButton.h"

UIDynamicButton::UIDynamicButton(UIAnchor anchor, const WindowPos &offsets,
    Callback callback, TextGetter getter) :
    UIButton(anchor, offsets, callback, getter()), getter(getter)
{}

UIDynamicButton::UIDynamicButton(UIAnchor anchor, UIElement *other,
    int padding, Callback callback, TextGetter getter) :
    UIButton(anchor, other, padding, callback, getter()), getter(getter)
{}

void UIDynamicButton::draw_text()
{
    label.set_text(getter());

    UIButton::draw_text();
}
