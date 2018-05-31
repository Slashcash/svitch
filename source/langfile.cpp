#include "langfile.hpp"

#include <fstream>
#include <sstream>

const std::string LangFile::SEPARATOR = "=";
LangFile* LangFile::instance = nullptr;
bool LangFile::is_initialized = false;

OPResult LangFile::loadFromFile(const std::string& thePath) {
    std::ostringstream initial_stream;
    initial_stream << "Starting the language file loading process at " << thePath;
    writeToLog(initial_stream.str());

    lang_map.clear();
    std::ifstream stream;
    stream.open(thePath.c_str(), std::ios::in);

    if( !stream.is_open() ) {
        OPResult op_res(ERR_OPEN_STREAM);
        writeToLog(op_res);
        return op_res;
    }

    writeToLog("Parsing the file");
    std::string temp_buffer;
    while( std::getline(stream, temp_buffer) ) {
        std::size_t separator_position = temp_buffer.find(SEPARATOR);
        if( separator_position == std::string::npos ) {  //try to find the separator in the line
            OPResult op_res(ERR_PARSE_FILE);
            writeToLog(op_res);
            return op_res;
        }

        std::string key = temp_buffer.substr(0, separator_position); //separate the key and the value using the separator position
        std::string value = temp_buffer.substr(separator_position+1, std::string::npos);
        lang_map.insert(std::make_pair(key, value));
    }

    writeToLog("language file loading SUCCESS");
    return OPResult(OPResult::SUCCESS);
}

std::string LangFile::getValue(const std::string& theKey) const {
    auto it = lang_map.find(theKey);
    if( it == lang_map.end() ) return "";
    else return it->second;
}

LangFile* LangFile::getInstance() {
    if( !is_initialized ) {
        instance = new LangFile();
        is_initialized = true;
    }

    return instance;
}
