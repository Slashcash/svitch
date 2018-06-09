#include "texture.hpp"

#include <fstream>
#include <picopng.hpp>
#include <nanojpeg.h>

#include "color.hpp"

OPResult Texture::loadFromMemory(void* theData, const std::size_t theSize) {
    OPResult op_res;

    if( decodeAsPNG(theData, theSize) ) { writeToLog("Texture loading SUCCESS"); return OPResult(OPResult::SUCCESS); }
    else if( (op_res = decodeAsJPEG(theData, theSize)) ) { writeToLog("Texture loading SUCCESS"); return OPResult(OPResult::SUCCESS); }
    else { writeToLog(op_res); return op_res; }
}

OPResult Texture::loadFromFile(const std::string& thePath) {
    writeToLog("Loading texture at "+thePath, 1);

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

    std::vector<unsigned char> image;
    image.resize((size_t)file_size);
    file.read((char*)(&image[0]), file_size);

    return loadFromMemory(&image[0], file_size);
}

OPResult Texture::decodeAsPNG(void* theInputData, const std::size_t theSize) {
    std::vector<unsigned char> buffer_data;

    int png_error;
    if( (png_error = decodePNG(buffer_data, width, height, (const unsigned char*)theInputData, theSize)) != 0 ) {
        OPResult op_res(ERR_DECODE_IMG, png_error);
        return op_res;
    }

    //converting it into a linear "framebuffer" of u32 for convenience reasons"
    for(unsigned int y = 0; y < getSize().y; y++ ) {
        for(unsigned int x = 0; x < getSize().x; x++  ) {
            unsigned int pos = (y * getSize().x + x) * 4;
            Color pixel_color(buffer_data[pos], buffer_data[pos+1], buffer_data[pos+2], buffer_data[pos+3]);
            texture_data.push_back(pixel_color);
        }
    }

    return OPResult(OPResult::SUCCESS);
}

OPResult Texture::decodeAsJPEG(void* theInputData, const std::size_t theSize) {
    njInit();

    nj_result_t jpeg_error;
    if( (jpeg_error = njDecode(theInputData, theSize)) != NJ_OK ) {
        njDone();
        OPResult op_res(ERR_DECODE_IMG, jpeg_error);
        return op_res;
    }

    width = njGetWidth();
    height = njGetHeight();
    unsigned char* buffer_data = njGetImage();

    //converting it into a linear "framebuffer" of u32 for convenience reasons"
    for(unsigned int y = 0; y < getSize().y; y++ ) {
        for(unsigned int x = 0; x < getSize().x; x++  ) {
            unsigned int pos = (y * getSize().x + x) * 3;
            Color pixel_color(buffer_data[pos], buffer_data[pos+1], buffer_data[pos+2], 255);
            texture_data.push_back(pixel_color);
        }
    }

    njDone();

    return OPResult(OPResult::SUCCESS);
}
