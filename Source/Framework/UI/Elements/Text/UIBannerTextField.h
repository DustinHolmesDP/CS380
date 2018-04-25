#pragma once
#include "UITextField.h"

// a text field that automatically sets its width to the screen size
class UIBannerTextField : public UITextField
{
public:
    UIBannerTextField(UIAnchor screenAnchor, const WindowPos &offsets,
        UIAnchor textAnchor, const std::wstring &text, TextColor color);
    UIBannerTextField(UIAnchor screenAnchor, UIElement *other, int padding,
        UIAnchor textAnchor, const std::wstring &text, TextColor color);

    virtual void update_position(const WindowSize &size) override;
protected:

private:
    void set_size(const WindowSize &size);
};