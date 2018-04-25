#include <pch.h>
#include "UIColorHighlighter.h"

UIColorHighlighter::UIColorHighlighter(const Color &base, const Color &highlighted,
    int duration, int inc, int dec) :
    base(base), highlighted(highlighted), adjuster(dec), current(0),
    maxTicks(duration), incAmount(inc), decAmount(dec)
{}

void UIColorHighlighter::start_highlighting()
{
    adjuster = incAmount;
}

void UIColorHighlighter::stop_highlighting()
{
    adjuster = decAmount;
}

void UIColorHighlighter::change_colors(const Color &b, const Color &h)
{
    base = b;
    highlighted = h;
}

Color UIColorHighlighter::get_color()
{
    current += adjuster;
    current = std::min(maxTicks, current);
    current = std::max(0, current);

    const float t = static_cast<float>(current) / static_cast<float>(maxTicks);

    return Color::Lerp(base, highlighted, t);
}
