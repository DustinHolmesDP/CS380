#pragma once
#include "UIButton.h"

class UIToggleButton : public UIButton
{
public:
    UIToggleButton(UIAnchor anchor, const WindowPos &offsets, Callback callback,
        const std::wstring &label, Getter<bool> state);
    UIToggleButton(UIAnchor anchor, UIElement *other, int padding, Callback callback,
        const std::wstring &label, Getter<bool> state);

    virtual void draw_sprite() override;
    virtual void draw_text() override;

protected:
    Getter<bool> state;

    void update_colors();
};