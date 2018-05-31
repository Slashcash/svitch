#include <algorithm>

#include "text.hpp"

Size Text::getSize() const {
    if( font == nullptr ) return Size(0, 0);

    else {
        //setting the correct font size
        font->setFontSize(font_size);

        unsigned int x_size = 0;
        std::vector<unsigned int> y_size;
        for(unsigned int i = 0; i < msg.size(); i++) {
            Size glyph_size;
            font->getGlyphData(msg[i], glyph_size);

            if( i != msg.size() - 1 ) x_size = x_size + font->getGlyphAdvanceSize().x;
            else x_size = x_size + glyph_size.x; //if it is the last character we just add its size, not its advance
            y_size.push_back(glyph_size.y); //we return the height of the glyph which has max height
        }

        return Size(x_size, *std::max_element(y_size.begin(), y_size.end()));
    }
}

void Text::drawCurrent(Window& theWindow, const Transformation& theTransformation) const {
    if( font != nullptr ) {
        //setting the correct font size
        font->setFontSize(font_size);

        int pen_x = 0;
        for(unsigned int i = 0; i < msg.size(); i++) {
            Size glyph_size;
            std::vector<unsigned char>* raw_data = font->getGlyphData(msg[i], glyph_size);

            for(unsigned int y = 0; y < glyph_size.y; y++ ) {
                for(unsigned int x = 0; x < glyph_size.x; x++  ) {
                    unsigned int pos = (y * glyph_size.x + x) * 4;
                    Color pixel_color((*raw_data)[pos], (*raw_data)[pos+1], (*raw_data)[pos+2], (*raw_data)[pos+3]);
                    theWindow.setWindowPixel(Coordinate(pen_x+font->getGlyphBitmapSize().x+x, y-font->getGlyphBitmapSize().y), pixel_color, theTransformation);
                }
            }

            pen_x = pen_x + font->getGlyphAdvanceSize().x;
        }
    }
}
