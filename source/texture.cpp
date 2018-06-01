#include "texture.hpp"

#include <fstream>
#include <picopng.hpp>

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

    int png_error;
    if( (png_error = decodePNG(texture_data, width, height, &image[0], image.size())) != 0 ) {
        OPResult op_res(ERR_DECODE_IMG);
        writeToLog(op_res);
        return op_res;
    }

    writeToLog("Texture loading SUCCESS");
    return OPResult(OPResult::SUCCESS);
}
