#include "rendertexture.hpp"

OPResult RenderTexture::loadFromFile(const std::string& thePath) {
    OPResult res = Texture::loadFromFile(thePath);
    render_size = Texture::getSize();
    draw_texture_data = texture_data;
    return res;
}

void RenderTexture::setPixelCurrent(const int thePosition, const Color& theColor) {
    draw_texture_data[thePosition] = theColor;
}
