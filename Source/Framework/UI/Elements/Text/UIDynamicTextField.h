#pragma once
#include "UITextField.h"

// a text field that queries its text every frame
class UIDynamicTextField : public UITextField
{
public:
    UIDynamicTextField(UIAnchor screenAnchor, const WindowPos &offsets,
        UIAnchor textAnchor, TextGetter getter, TextColor color);
    UIDynamicTextField(UIAnchor screenAnchor, UIElement *other, int padding,
        UIAnchor textAnchor, TextGetter getter, TextColor color);

    virtual void draw_text() override;

protected:
    TextGetter getter;
};