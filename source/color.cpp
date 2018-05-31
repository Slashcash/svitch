#include "color.hpp"

Color::Color(const u8 theRed, const u8 theGreen, const u8 theBlue, const u8 theAlpha) {
    rgb_color = RGBA8(theRed, theGreen, theBlue, theAlpha);
}

Color::Color(const u32 theColor) {
    rgb_color = theColor;
}
