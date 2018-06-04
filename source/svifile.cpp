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
    writeToLog(initial_stream.str(), 1);

    //we will now extract our headerfile into a string and read its information
    //we should use this painful c approach because switch is a bitch

    //reading the entire archive :(
    writeToLog("Validating the archive");
    FILE* src = fopen(file_path.c_str(), "rb");
    if( src == NULL ) {
        is_valid = false;
        title_id = 0;
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_names.push_back("");
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_authors.push_back("");
        OPResult op_res(ERR_OPEN_STREAM);
        writeToLog(op_res);
        return;
    }

    fseek(src, 0, SEEK_END);
    u64 file_size = ftell(src);
    rewind(src);

    const int BUFFER_SIZE = 0x5000;
    size_t read_size;
    char* buf = new char[BUFFER_SIZE];
    std::string data;

    while( (read_size = fread(buf, 1, BUFFER_SIZE, src)) > 0 ) data.append(buf, read_size);
    delete [] buf;

    if( data.size() != file_size ) {
        is_valid = false;
        title_id = 0;
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_names.push_back("");
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_authors.push_back("");
        OPResult op_res(ERR_READ_FILE);
        writeToLog(op_res);
        ;return;
    }

    //initializing an archive
    mz_zip_archive archive;
    mz_zip_zero_struct(&archive);
    if( !mz_zip_reader_init_mem(&archive, (void*)data.c_str(), file_size, 0) ) {
        is_valid = false;
        title_id = 0;
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_names.push_back("");
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_authors.push_back("");
        OPResult op_res(ERR_INIT_ARCHIVE);
        writeToLog(op_res);
        return;
    }

    size_t header_size = 0;
    char* buffer;

    writeToLog("Reading the header");
    if( (buffer = (char*)mz_zip_reader_extract_file_to_heap(&archive, SaveFile::DEFAULT_SAVEHEADER_NAME.c_str(), &header_size, 0)) == NULL ) {
        is_valid = false;
        title_id = 0;
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_names.push_back("");
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_authors.push_back("");
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
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_names.push_back("");
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) title_authors.push_back("");
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
    for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++) {
        std::ostringstream NAME_TO_FIND;
        NAME_TO_FIND << SaveFile::HEADER_NAME_STR << "_" << i;

        std::ostringstream AUTHOR_TO_FIND;
        AUTHOR_TO_FIND << SaveFile::HEADER_AUTHOR_STR << "_" << i;

        element_pos = str_buffer.find(NAME_TO_FIND.str());
        if( element_pos != std::string::npos ) {
            newline_pos = str_buffer.find("\n", element_pos);
            value_pos = element_pos + NAME_TO_FIND.str().size() + SaveFile::HEADER_SEPARATOR.size();
            if( newline_pos == std::string::npos ) value_length = std::string::npos;
            else value_length = newline_pos - element_pos - NAME_TO_FIND.str().size() - SaveFile::HEADER_SEPARATOR.size();
            title_names.push_back(str_buffer.substr(value_pos, value_length));
        }

        else title_names.push_back("");

        //searching for the author in the header
        element_pos = str_buffer.find(AUTHOR_TO_FIND.str());
        if( element_pos != std::string::npos ) {
            newline_pos = str_buffer.find("\n", element_pos);
            value_pos = element_pos + AUTHOR_TO_FIND.str().size() + SaveFile::HEADER_SEPARATOR.size();
            if( newline_pos == std::string::npos ) value_length = std::string::npos;
            else value_length = newline_pos - element_pos - AUTHOR_TO_FIND.str().size() - SaveFile::HEADER_SEPARATOR.size();
            title_authors.push_back(str_buffer.substr(value_pos, value_length));
        }

        else title_authors.push_back("");
    }

    //SEARCHING FOR THE ENGLISH LANGUAGES TO WRITE THE LOG CORRECTLY
    std::ostringstream log_str_final;
    std::string log_temp_title;
    std::string log_temp_author;
    if( title_names[1].empty() ) log_temp_title = "NO_ENGLISH";
    else log_temp_title = title_names[1];

    if( title_authors[1].empty() ) log_temp_author = "NO_ENGLISH";
    else log_temp_author = title_authors[1];

    log_str_final << "Additional information found. " <<  "Title " << std::hex << title_id << " is " << log_temp_title << ", " << log_temp_author;
    writeToLog(log_str_final.str());
}

std::vector<SVIFile> SVIFile::getAllSVIInPath(const std::string& thePath) {
    std::vector<SVIFile> buffer;

    //we sanitize the path (if it does not end with a slash, we add it)
    std::string sanitized_path = thePath;
    if( thePath.find_last_of("/") != thePath.size()-1 ) sanitized_path.append("/");

    std::ostringstream initial_stream;
    initial_stream << "Starting a .svi scan at " << sanitized_path;
    writeToLog(initial_stream.str(), 1);

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

std::string SVIFile::getBestSuitableName(const s32 theLanguage) const {
    if( theLanguage >= SaveFile::LANGUAGE_NUM ) return "UNKNOWN";

    //converting theLanguage into the vector index code (why you are so stupid Nintendo?)
    int index;
    if( theLanguage == 0 ) index = 2; //JAPANESE
    else if( theLanguage == 1 ) index = 0; //AMERICAN ENGLISH
    else if( theLanguage == 2 ) index = 3; //FRENCH
    else if( theLanguage == 3 ) index = 4; //GERMAN
    else if( theLanguage == 4 ) index = 7; //ITALIAN
    else if( theLanguage == 5 ) index = 6; //SPANISH
    else if( theLanguage == 6 ) index = 14; //CHINESE
    else if( theLanguage == 7 ) index = 12; //KOREAN
    else if( theLanguage == 8 ) index = 8; //DUTCH
    else if( theLanguage == 9 ) index = 10; //PORTUGUESE
    else if( theLanguage == 10 ) index = 11; //RUSSIAN
    else index = 0; //THERE ARE OTHER LANGUAGES BUT I'M SO BORED AND NOBODY WILL USE THIS ANYWAY, IF SOME TAIWANESE PEOPLE NEEDS THIS YOU ARE FREE TO CONTACT ME

    if( !title_names[index].empty() ) return title_names[index]; //if we have a string for the language we return it

    else
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++)
            if( !title_names[i].empty() )
                return title_names[i];

    return "UNKNOWN";
}

std::string SVIFile::getBestSuitableAuthor(const s32 theLanguage) const {
    if( theLanguage >= SaveFile::LANGUAGE_NUM ) return "UNKNOWN";

    //converting theLanguage into the vector index code (why you are so stupid Nintendo?)
    int index;
    if( theLanguage == 0 ) index = 2; //JAPANESE
    else if( theLanguage == 1 ) index = 0; //AMERICAN ENGLISH
    else if( theLanguage == 2 ) index = 3; //FRENCH
    else if( theLanguage == 3 ) index = 4; //GERMAN
    else if( theLanguage == 4 ) index = 7; //ITALIAN
    else if( theLanguage == 5 ) index = 6; //SPANISH
    else if( theLanguage == 6 ) index = 14; //CHINESE
    else if( theLanguage == 7 ) index = 12; //KOREAN
    else if( theLanguage == 8 ) index = 8; //DUTCH
    else if( theLanguage == 9 ) index = 10; //PORTUGUESE
    else if( theLanguage == 10 ) index = 11; //RUSSIAN
    else index = 0; //THERE ARE OTHER LANGUAGES BUT I'M SO BORED AND NOBODY WILL USE THIS ANYWAY, IF SOME TAIWANESE PEOPLE NEEDS THIS YOU ARE FREE TO CONTACT ME

    if( !title_authors[index].empty() ) return title_authors[index]; //if we have a string for the language we return it

    else
        for(unsigned int i = 0; i < SaveFile::LANGUAGE_NUM; i++)
            if( !title_authors[i].empty() )
                return title_authors[i];

    return "UNKNOWN";
}
