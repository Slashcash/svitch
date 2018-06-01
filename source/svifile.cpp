#include "svifile.hpp"
#include <sstream>
#include <miniz.h>
#include <dirent.h>

#include "savefile.hpp"

SVIFile::SVIFile(const std::string& thePath) {
    file_path = thePath;
    is_valid = true;

    getInformations(); //getting some more informations about the file
}

void SVIFile::getInformations() {
    std::ostringstream initial_stream;
    initial_stream << "Starting the loading process for the .svi file at " << file_path;
    writeToLog(initial_stream.str());

    //we will now extract our headerfile into a string and read its information
    size_t header_size = 0;
    char* buffer;

    writeToLog("Reading the header");
    if( (buffer = (char*)mz_zip_extract_archive_file_to_heap(file_path.c_str(), SaveFile::DEFAULT_SAVEHEADER_NAME.c_str(), &header_size, 0)) == NULL ) {
        is_valid = false;
        title_id = 0;
        title_name = SaveFile::UNKNOWN_PARAMETER_STR;
        title_author = SaveFile::UNKNOWN_PARAMETER_STR;
        OPResult op_res(ERR_NO_SAVEHEADER);
        writeToLog(op_res);
        return;
    }

    std::string str_buffer;
    str_buffer.append(buffer, header_size);

    delete [] buffer;

    std::size_t element_pos;
    std::size_t value_pos;
    std::size_t value_length;
    std::size_t newline_pos;

    //searching for the id in the header
    element_pos = str_buffer.find(SaveFile::HEADER_ID_STR);
    if( element_pos == std::string::npos ) {
        OPResult op_res(ERR_INVALID_SAVEHEADER);
        writeToLog(op_res);
        title_id = 0;
        title_name = SaveFile::UNKNOWN_PARAMETER_STR;
        title_author = SaveFile::UNKNOWN_PARAMETER_STR;
        is_valid = false;
        return;
    }

    newline_pos = str_buffer.find("\n", element_pos);
    value_pos = element_pos + SaveFile::HEADER_ID_STR.size() + SaveFile::HEADER_SEPARATOR.size();
    if( newline_pos == std::string::npos ) value_length = std::string::npos;
    else value_length = newline_pos - element_pos - SaveFile::HEADER_ID_STR.size() - SaveFile::HEADER_SEPARATOR.size(); //a -1 should go here based on my calculations (:D) but it doesn't work with it
    std::string temp = str_buffer.substr(value_pos, value_length);
    title_id = std::strtoul(temp.c_str(), nullptr, 10);

    //searching for the name in the header
    element_pos = str_buffer.find(SaveFile::HEADER_NAME_STR);
    if( element_pos != std::string::npos ) {
        newline_pos = str_buffer.find("\n", element_pos);
        value_pos = element_pos + SaveFile::HEADER_NAME_STR.size() + SaveFile::HEADER_SEPARATOR.size();
        if( newline_pos == std::string::npos ) value_length = std::string::npos;
        else value_length = newline_pos - element_pos - SaveFile::HEADER_NAME_STR.size() - SaveFile::HEADER_SEPARATOR.size();
        title_name = str_buffer.substr(value_pos, value_length);
    }

    else {
        OPResult op_res(ERR_INVALID_SAVEHEADER);
        writeToLog(op_res, LogWriter::WARNING);
        title_name = SaveFile::UNKNOWN_PARAMETER_STR;
    }
    //searching for the author in the header
    element_pos = str_buffer.find(SaveFile::HEADER_AUTHOR_STR);
    if( element_pos != std::string::npos ) {
        newline_pos = str_buffer.find("\n", element_pos);
        value_pos = element_pos + SaveFile::HEADER_AUTHOR_STR.size() + SaveFile::HEADER_SEPARATOR.size();
        if( newline_pos == std::string::npos ) value_length = std::string::npos;
        else value_length = newline_pos - element_pos - SaveFile::HEADER_AUTHOR_STR.size() - SaveFile::HEADER_SEPARATOR.size();
        title_author = str_buffer.substr(value_pos, value_length);
    }

    else {
        OPResult op_res(ERR_INVALID_SAVEHEADER);
        writeToLog(op_res, LogWriter::WARNING);
        title_author = SaveFile::UNKNOWN_PARAMETER_STR;
    }

    std::ostringstream final_stream;
    final_stream << ".svi loading SUCCESS, " << file_path << " is " << title_id <<", " << title_name << " - " << title_author;
    writeToLog(final_stream.str());
}

std::vector<SVIFile> SVIFile::getAllSVIInPath(const std::string& thePath) {
    std::vector<SVIFile> buffer;

    //we sanitize the path (if it does not end with a slash, we add it)
    std::string sanitized_path = thePath;
    if( thePath.find_last_of("/") != thePath.size()-1 ) sanitized_path.append("/");

    std::ostringstream initial_stream;
    initial_stream << "Starting a .svi scan at " << sanitized_path;
    writeToLog(initial_stream.str());

    DIR* d = opendir(sanitized_path.c_str()); // open the path
    if( d == NULL ) {
        OPResult op_res(ERR_OPEN_ITERATOR);
        writeToLog(op_res);
        return buffer; // if was not able return
    }

    dirent* dir; // for the directory entries

    while((dir = readdir(d)) != NULL) {
        if( dir->d_type == DT_REG ) {
            std::string file_name = dir->d_name;
            std::size_t point_pos = file_name.find_last_of(".");
            std::string file_extension;
            if( point_pos != std::string::npos ) file_extension = file_name.substr(point_pos + 1, std::string::npos); //searching for the .svi extension

            if( file_extension == "svi" ) {
                writeToLog(std::string("Found ")+dir->d_name);
                buffer.push_back(SVIFile(sanitized_path+dir->d_name));
            }
        }
    }

    std::ostringstream final_stream;
    final_stream << "SVI file scan SUCCESS. Found " << buffer.size() << " files";
    return buffer;
}
