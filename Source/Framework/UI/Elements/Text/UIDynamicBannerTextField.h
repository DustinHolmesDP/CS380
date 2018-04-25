#pragma once
#include "UIBannerTextField.h"

// a text field that automatically sets its width to the screen size, and queries the text every frame
class UIDynamicBannerTextField : public UIBannerTextField
{
public:
    UIDynamicBannerTextField(UIAnchor screenAnchor, const WindowPos &offsets,
        UIAnchor textAnchor, TextGetter getter, TextColor color);
    UIDynamicBannerTextField(UIAnchor screenAnchor, UIElement *other, int padding,
        UIAnchor textAnchor, TextGetter getter, TextColor color);

    virtual void draw_text() override;

protected:
    TextGetter getter;
};