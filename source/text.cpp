#include <algorithm>

#include "text.hpp"

Text::Text() : Node() {
    font = nullptr;
    font_size = Font::DEFAULT_SCALABLE_SIZE;
    font_color = Color::WHITE;
    fixed_width = 0;
    end_position = msg.size();
}

Text::Text(Font& theFont, const std::string& theMsg) : Node() {
    font = &theFont;
    msg = convertToUTF8(theMsg);
    font_size = Font::DEFAULT_SCALABLE_SIZE;
    font_color = Color::WHITE;
    fixed_width = 0;
    end_position = msg.size();
}

Size Text::getSizeByIndex(const unsigned int theStartPosition, const unsigned int theEndPosition) const {
    if( font == nullptr ) return Size(0, 0);

    else {
        //setting the correct font size
        font->setFontSize(font_size);

        unsigned int x_size = 0;
        std::vector<unsigned int> y_size;
        y_size.push_back(0); //pushing 0 just in case it ends up being empty
        for(unsigned int i = theStartPosition; i < theEndPosition; i++) {
            Size glyph_size;

            font->getGlyphData(msg[i], glyph_size);

            if( i != msg.size() - 1 ) x_size = x_size + font->getGlyphAdvanceSize().x;
            else x_size = x_size + glyph_size.x; //if it is the last character we just add its size, not its advance
            y_size.push_back(glyph_size.y); //we return the height of the glyph which has max height
        }

        return Size(x_size, *std::max_element(y_size.begin(), y_size.end()));
    }
}

Size Text::getSize() const {
    Size temp = getSizeByIndex(0, msg.size());

    if( fixed_width != 0 ) return Size(fixed_width, temp.y);
    else return temp;
}

void Text::drawCurrent(RenderSurface& theTarget, const Transformation& theTransformation) const {
    if( font != nullptr ) {
        //setting the correct font size
        font->setFontSize(font_size);

        int pen_x = 0;
        for(unsigned int i = 0; i < end_position; i++) {
            Size glyph_size;

            std::vector<unsigned char>* raw_data = font->getGlyphData(msg[i], glyph_size);

            //alpha blending
            float opacity;
            for(unsigned int y = 0; y < glyph_size.y; y++ ) {
                for(unsigned int x = 0; x < glyph_size.x; x++  ) {
                    Color background_color = theTarget.getPixel(Coordinate(pen_x+font->getGlyphBitmapSize().x+x, y-font->getGlyphBitmapSize().y), theTransformation);
                    unsigned int pos = (y * glyph_size.x + x) * 4;

                    if( (*raw_data)[pos] == 0 ) theTarget.setPixel(Coordinate(pen_x+font->getGlyphBitmapSize().x+x, y-font->getGlyphBitmapSize().y), background_color, theTransformation);
                    else {
                        //calculate opacity
                        opacity = (*raw_data)[pos] / 255.0;
                        u8 red_value = font_color.getRed() * opacity + (1 - opacity) * background_color.getRed();
                        u8 green_value = font_color.getGreen() * opacity + (1 - opacity) * background_color.getGreen();
                        u8 blue_value = font_color.getBlue() * opacity + (1 - opacity) * background_color.getBlue();
                        theTarget.setPixel(Coordinate(pen_x+font->getGlyphBitmapSize().x+x, y-font->getGlyphBitmapSize().y), Color(red_value, green_value, blue_value, 255), theTransformation);
                    }
                }
            }

            pen_x = pen_x + font->getGlyphAdvanceSize().x;
        }
    }
}

std::vector<uint32_t> Text::convertToUTF8(const std::string& theMSG) {
    std::vector<uint32_t> buffer;

    for(unsigned int i = 0; i < theMSG.size(); ) {
        Size glyph_size;

        //converting to utf-8
        uint32_t utf8_char;
        ssize_t skip = decode_utf8(&utf8_char,(uint8_t*)(&theMSG[i]));
        if( skip <= 0 ) break;

        buffer.push_back(utf8_char);
        i = i + skip;
    }

    return buffer;
}

void Text::calculateFixedWidth() {
    if( fixed_width > 0 ) {
        std::size_t temp_end_position = msg.size()+1; //past the last element
        unsigned int temp_width = 0;
        do {
            temp_end_position--;
            temp_width = getSizeByIndex(0, temp_end_position).x;
        }while(temp_width > fixed_width && temp_end_position != 0);

        end_position = temp_end_position;
    }

    else end_position = msg.size();
}
