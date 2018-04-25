#pragma once
#include "UIButton.h"

// a button that has dynamic text
class UIDynamicButton : public UIButton
{
public:
    UIDynamicButton(UIAnchor anchor, const WindowPos &offsets, Callback callback, TextGetter getter);
    UIDynamicButton(UIAnchor anchor, UIElement *other, int padding, Callback callback, TextGetter getter);

    virtual void draw_text() override;

protected:
    TextGetter getter;
};