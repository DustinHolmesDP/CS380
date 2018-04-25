#include <pch.h>
#include "UIDynamicTextField.h"

UIDynamicTextField::UIDynamicTextField(UIAnchor screenAnchor, const WindowPos &offsets, 
    UIAnchor textAnchor, TextGetter getter, TextColor color) : 
    UITextField(screenAnchor, offsets, textAnchor, getter(), color), getter(getter)
{}

UIDynamicTextField::UIDynamicTextField(UIAnchor screenAnchor, UIElement *other,
    int padding, UIAnchor textAnchor, TextGetter getter, TextColor color) : 
    UITextField(screenAnchor, other, padding, textAnchor, getter(), color), getter(getter)
{}

void UIDynamicTextField::draw_text()
{
    text.set_text(getter());
    UITextField::draw_text();
}