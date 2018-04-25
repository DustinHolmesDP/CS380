#pragma once
#include "UIButton.h"

// a button that will only be visible/clickable if a condition is true
class UIConditionalButton : public UIButton
{
public:
    UIConditionalButton(UIAnchor anchor, const WindowPos &offsets, Callback callback,
        const std::wstring &label, Getter<bool> condition);
    UIConditionalButton(UIAnchor anchor, UIElement *other, int padding, Callback callback,
        const std::wstring &label, Getter<bool> condition);

    virtual void draw_sprite() override;
    virtual void draw_text() override;
    virtual bool in_element_specific_space(const WindowPos &point) const override;

protected:
    Getter<bool> condition;
};