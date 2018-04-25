#pragma once
#include "UITextField.h"

// a text field that displays a label and a value that is queried every frame
class UIValueTextField : public UITextField
{
public:
    UIValueTextField(UIAnchor anchor, const WindowPos &offsets, const std::wstring &label, 
        TextGetter getter, TextColor color);
    UIValueTextField(UIAnchor anchor, UIElement *other, int padding,
        const std::wstring &label, TextGetter getter, TextColor color);

    virtual void draw_text() override;
protected:
    UITextObject valueText;
    TextGetter getter;
};