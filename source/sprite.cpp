#include "sprite.hpp"

#include "color.hpp"

void Sprite::drawCurrent(RenderSurface& theTarget, const Transformation& theTransformation) const {
    if( texture != nullptr) {
        const std::vector<u32>* raw_data = texture->getRawTextureData();;

         for(unsigned int y = 0; y < getSize().y; y++ ) {
            for(unsigned int x = 0; x < getSize().x; x++  ) {
                unsigned int pos = (y * getSize().x + x);
                theTarget.setPixel(Coordinate(x, y), (*raw_data)[pos], theTransformation);
            }
        }
    }
}
