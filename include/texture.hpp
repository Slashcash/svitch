#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include <vector>

#include "opresult.hpp"
#include "transformation.hpp"
#include "loadable.hpp"
#include "logwriter.hpp"

class Texture : public Loadable, public LogWriter {
    private:
        std::vector<unsigned char> texture_data; //rgba8 texture data decoded using picopng
        unsigned long width;
        unsigned long height;

    public:
        Texture() { width = 0; height = 0; }

        Size getSize() const { return Size(width, height); }

        OPResult loadFromFile(const std::string& thePath); //loads a texture from a file
        const std::vector<u8>* getRawTextureData() const { return &texture_data; } //gets the raw RGBA8 data for this texture
};

#endif // _TEXTURE_HPP_
