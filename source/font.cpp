#include "font.hpp"

#include <switch.h>
#include <sstream>

bool Font::freetype_initialized = false;
FT_Library Font::freetype_library;
const std::string Font::SHARED_FONT = "SHARED_FONT";

Font::Font() {
    font_loaded = false;

    if( !freetype_initialized ) {
        FT_Error error = FT_Init_FreeType(&freetype_library);
        if( error ) {
            OPResult op_res(ERR_FREETYPE_NOT_INITIALIZED, error);
            writeToLog(op_res);
            return;
        }

        else { freetype_initialized = true; }
    }
}

OPResult Font::loadFromMemory(void* theData, const std::size_t theSize) {
    if( !freetype_initialized ) {
        OPResult op_res(ERR_FREETYPE_NOT_INITIALIZED);
        writeToLog(op_res);
        return op_res;
    }

    FT_Error error = FT_New_Memory_Face(freetype_library, (const unsigned char*)theData, theSize, 0, &font_face);
        if( error ) {
        OPResult op_res(ERR_LOAD_FONT_FACE, error);
        writeToLog(op_res);
        return op_res;
    }

    font_loaded = true;

    //choosing the font size
    unsigned int default_size;
    if( (font_face->face_flags & FT_FACE_FLAG_SCALABLE) ) default_size = DEFAULT_SCALABLE_SIZE; //if the font is scalable we choose an arbitrary size
    else if( (font_face->face_flags & FT_FACE_FLAG_FIXED_SIZES) && (font_face->num_fixed_sizes > 0) ) default_size = font_face->available_sizes[0].width;
    else default_size = DEFAULT_SCALABLE_SIZE; //if we reach here we are basicly fucked

    std::ostringstream size_stream;
    size_stream << "Setting the font size to " << default_size;
    writeToLog(size_stream.str());
    OPResult res = setFontSize(default_size);
    if( !res ) return res;

    writeToLog("Loading font SUCCESS");
    return OPResult(OPResult::SUCCESS);
}

OPResult Font::loadFromFile(const std::string& thePath) {
    if( thePath == SHARED_FONT ) return loadSharedFont();

    else {
        std::ostringstream initial_stream;
        initial_stream << "Loading a font from " << thePath;
        writeToLog(initial_stream.str(), 1);

        if( !freetype_initialized ) {
            OPResult op_res(ERR_FREETYPE_NOT_INITIALIZED);
            writeToLog(op_res);
            return op_res;
        }

        std::ifstream file(thePath.c_str(), std::iostream::in | std::ifstream::binary | std::ifstream::ate);

        if( !file.is_open() ) {
            OPResult op_res(ERR_OPEN_STREAM);
            writeToLog(op_res);
            return op_res;
        }

        std::streamsize file_size = 0;
        if(file.seekg(0, std::ios::end).good()) file_size = file.tellg();
        if(file.seekg(0, std::ios::beg).good()) file_size -= file.tellg();

        if( file_size <= 0 ) {
            OPResult op_res(ERR_READ_FILE);
            writeToLog(op_res);
            return op_res;
        }

        std::vector<unsigned char> font;
        font.resize((size_t)file_size);
        file.read((char*)(&font[0]), file_size);

        return loadFromMemory(&font[0], file_size);
    }
}

OPResult Font::setFontSize(const unsigned int theSize) {
    if( !freetype_initialized ) return OPResult(ERR_FREETYPE_NOT_INITIALIZED);


    if( !font_loaded ) return OPResult(ERR_FONT_NOT_LOADED);


    FT_Error error = FT_Set_Pixel_Sizes(font_face, theSize, 0);
    if( error ) return OPResult(ERR_SET_FONT_SIZE);

    else return OPResult(OPResult::SUCCESS);
}

std::vector<unsigned char>* Font::getGlyphData(const uint32_t theCharacter, Size& theSize) {
    glyph_data.clear();

    if( !freetype_initialized ) { theSize = Size(0, 0); return nullptr; }
    if( !font_loaded ) { theSize = Size(0, 0); return nullptr; }

    FT_Error error = FT_Load_Char(font_face, theCharacter, FT_LOAD_RENDER);
    if ( error ) { theSize = Size(0, 0); return nullptr; }

    //loading it into our vector
    u8* buffer = font_face->glyph->bitmap.buffer;
    u32 width = font_face->glyph->bitmap.width;
    u32 height = font_face->glyph->bitmap.rows;

    if( font_face->glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY ) { theSize = Size(0, 0); return nullptr; }


    for (u32 y = 0; y < height; y++) {
        for (u32 x = 0; x < width; x++) {
            glyph_data.push_back(buffer[x]);
            glyph_data.push_back(buffer[x]);
            glyph_data.push_back(buffer[x]);
            glyph_data.push_back(255); //pushing max alpha
        }

        buffer += font_face->glyph->bitmap.pitch;
    }

    theSize = Size(font_face->glyph->metrics.width / 64, font_face->glyph->metrics.height / 64);
    return &glyph_data;
}

Size Font::getGlyphAdvanceSize() const {
    if( !freetype_initialized ) { return Size(0, 0); }
    if( !font_loaded ) { return Size(0, 0); }

    return Size(font_face->glyph->advance.x >> 6, font_face->glyph->advance.y >> 6);
}

Size Font::getGlyphBitmapSize() const {
    if( !freetype_initialized ) { return Size(0, 0); }
    if( !font_loaded ) { return Size(0, 0); }

    return Size(font_face->glyph->bitmap_left, font_face->glyph->bitmap_top);
}

OPResult Font::loadSharedFont() {
    PlFontData font;
    writeToLog("Loading the shared font", 1);

    if( !freetype_initialized ) {
        OPResult op_res(ERR_FREETYPE_NOT_INITIALIZED);
        writeToLog(op_res);
        return op_res;
    }

    writeToLog("Initializing pl");
    Result res = plInitialize();
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_INITIALIZE_PL, R_DESCRIPTION(res));
        writeToLog(op_res);
        return op_res;
    }

    writeToLog("Getting the font");
    res = plGetSharedFontByType(&font, PlSharedFontType_Standard);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_GET_SHARED_FONT, R_DESCRIPTION(res));
        writeToLog(op_res);
        plExit();
        return op_res;
    }

    OPResult op_res = loadFromMemory(font.address, font.size);
    plExit();
    return op_res;
}
