#include "sprite.hpp"

#include "color.hpp"

void Sprite::drawCurrent(Window& theWindow, const Transformation& theTransformation) const {
    if( texture != nullptr) {
        const std::vector<u8>* raw_data = texture->getRawTextureData();;

         for(unsigned int y = 0; y < getSize().y; y++ ) {
            for(unsigned int x = 0; x < getSize().x; x++  ) {
                unsigned int pos = (y * getSize().x + x) * 4;
                Color pixel_color((*raw_data)[pos], (*raw_data)[pos+1], (*raw_data)[pos+2], (*raw_data)[pos+3]);
                theWindow.setWindowPixel(Coordinate(x, y), pixel_color, theTransformation);
            }
        }
    }
}
