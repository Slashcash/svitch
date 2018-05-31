#ifndef _COLOR_HPP_
#define _COLOR_HPP_

//this abstracts the concept of a color for this library

#include "switch.h"

class Color {
    private:
        u32 rgb_color; //this color in RGB888

    public:
        static constexpr u32 RED = RGBA8_MAXALPHA(255, 0, 0);
        static constexpr u32 GREEN = RGBA8_MAXALPHA(0, 255, 0);
        static constexpr u32 BLUE = RGBA8_MAXALPHA(0, 0, 255);
        static constexpr u32 WHITE = RGBA8_MAXALPHA(255, 255, 255);
        static constexpr u32 BLACK = RGBA8_MAXALPHA(0, 0, 0);

        Color() { rgb_color = WHITE; }
        Color(const u8 theRed, const u8 theGreen, const u8 theBlue, const u8 theAlpha);
        Color(const u32 theColor); //to be used with provided constants

        operator u32() const { return rgb_color; }

};



#endif // _COLOR_HPP_
