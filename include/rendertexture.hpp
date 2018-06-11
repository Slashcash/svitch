#ifndef _RENDERTEXTURE_HPP_
#define _RENDERTEXTURE_HPP_

/* this is a special texture, you can draw on it, YAY! be aware that this is an heavy object, generally it takes double the amount of RAM of a normal texture*/

#include "rendersurface.hpp"
#include "texture.hpp"

class RenderTexture : public RenderSurface, public Texture {
    private:
        std::vector<u32> original_texture_data;

        virtual void setPixelCurrent(const int thePosition, const Color& theColor);
        virtual Color getPixelCurrent(const int thePosition) { return texture_data[thePosition]; }

    public:
        RenderTexture() : Texture() { render_size = Size(0, 0); }
        OPResult loadFromFile(const std::string& thePath); //loads a texture from a file
        void clear() { texture_data = original_texture_data; }
        const std::vector<u32>* getRawTextureData() const { return &texture_data; } //gets the raw RGBA8 data for this texture

};

#endif
