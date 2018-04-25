#pragma once
#include <string>
#include "UIAnchor.h"
#include "Misc/NiceTypes.h"
#include <memory>
#include <SpriteFont.h>

class UIElement;
class UICoordinator;

class UITextObject
{
    friend class UICoordinator;
public:
    UITextObject(UIAnchor anchor, const std::wstring &text);
    UITextObject(UIAnchor anchor, const wchar_t *text);

    void set_text(const std::wstring &newText);
    void set_text(const wchar_t *text);

    void draw(const UIBoundingBox &bounds, TextColor color = TextColor::BLACK);

private:
    UIAnchor anchor;
    std::wstring text;
};