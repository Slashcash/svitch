#ifndef _FONT_HPP_
#define _FONT_HPP_

#include <vector>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "opresult.hpp"
#include "loadable.hpp"
#include "transformation.hpp"
#include "logwriter.hpp"

/*A font for this library, it heavily uses libfreetype*/

class Font : public Loadable, public LogWriter {
    private:
        static bool freetype_initialized;
        static FT_Library freetype_library;
        FT_Face font_face;
        bool font_loaded;
        std::vector<unsigned char> glyph_data;

    public:
        static const unsigned int DEFAULT_SCALABLE_SIZE = 20; //the default size for a scalable font

        Font();

        bool isLoaded() const { return font_loaded; }
        std::vector<unsigned char>* getGlyphData(const uint32_t theCharacter, Size& theSize); //returns raw glyph RGBA8 data, theSize is filled with its size
        Size getGlyphAdvanceSize() const;
        Size getGlyphBitmapSize() const;

        OPResult setFontSize(const unsigned int theSize);

        virtual OPResult loadFromFile(const std::string& thePath);
};



#endif // _FONT_HPP_
