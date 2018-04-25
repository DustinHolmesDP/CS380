#include <pch.h>
#include "UIDynamicBannerTextField.h"

UIDynamicBannerTextField::UIDynamicBannerTextField(UIAnchor screenAnchor,
    const WindowPos &offsets, UIAnchor textAnchor, TextGetter getter, TextColor color) :
    UIBannerTextField(screenAnchor, offsets, textAnchor, getter(), color), getter(getter)
{}

UIDynamicBannerTextField::UIDynamicBannerTextField(UIAnchor screenAnchor, 
    UIElement *other, int padding, UIAnchor textAnchor, TextGetter getter, TextColor color) :
    UIBannerTextField(screenAnchor, other, padding, textAnchor, getter(), color), getter(getter)
{}

void UIDynamicBannerTextField::draw_text()
{
    text.set_text(getter());
    UITextField::draw_text();
}
