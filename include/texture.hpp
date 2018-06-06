#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include <vector>

#include "opresult.hpp"
#include "transformation.hpp"
#include "loadable.hpp"
#include "logwriter.hpp"

class Texture : public Loadable, public LogWriter {
    protected: //i know not elegant but this is so convenient for RenderTexture
        std::vector<u32> texture_data; //rgba8 texture decoded data

    private:
        unsigned long width;
        unsigned long height;

        OPResult decodeAsPNG(void* theInputData, const std::size_t theSize);
        OPResult decodeAsJPEG(void* theInputData, const std::size_t theSize);

    public:
        Texture() { width = 0; height = 0; }

        Size getSize() const { return Size(width, height); }

        OPResult loadFromFile(const std::string& thePath); //loads a texture from a file
        OPResult loadFromMemory(void* theData, const std::size_t theSize);
        const std::vector<u32>* getRawTextureData() const { return &texture_data; } //gets the raw RGBA8 data for this texture
};

#endif // _TEXTURE_HPP_
