#include "savefile.hpp"

#ifdef EMULATOR
#include <cstdlib>
#include <dirent.h>
#else
#include <switch.h>
#include <unistd.h>
#endif
#include <sstream>
#include <fstream>
#include <string.h>
#include <dirent.h>

#ifdef EMULATOR
const std::string SaveFile::DEFAULT_SAVEPATH = "sdmc:/.saves/";
#else
const std::string SaveFile::DEFAULT_MOUNTNAME = "svitch_save";
#endif
const std::string SaveFile::DEFAULT_SAVEHEADER_NAME = "svitch_saveheader.svh";
const std::string SaveFile::HEADER_SEPARATOR = "=";
const std::string SaveFile::UNKNOWN_PARAMETER_STR = "Unknown";
const std::string SaveFile::HEADER_ID_STR = "id";
const std::string SaveFile::HEADER_NAME_STR = "name";
const std::string SaveFile::HEADER_AUTHOR_STR = "author";

#ifdef EMULATOR
SaveFile::SaveFile(const std::string& thePath) {
    title_id = 0;

    //sanitizing the string input with a slash at the end
     folder_path = thePath;
     if( folder_path[folder_path.size()-1] != '/' ) folder_path.append("/");

    getSaveFileInformation();
}
#else
SaveFile::SaveFile(const u64 theTitleID, const u128 theUserID) {
    title_id = theTitleID;
    user_id = theUserID;

    //retrieving some more savefile information
    getSaveFileInformation();
    #ifndef EMULATOR
    getUserInformation();
    #endif
}
#endif

void SaveFile::getSaveFileInformation() {
    //this function is different if launched on emulator or real hardware
    #ifndef EMULATOR
    std::ostringstream log_str;
    log_str << "Getting additional information for " << std::hex << title_id;
    writeToLog(log_str.str(), 1);

    NsApplicationControlData* buf = nullptr;
    unsigned int langentry = 1; //this is the id for the english language
    size_t outsize=0;

    char name[0x201];
    char author[0x101];

    buf = new NsApplicationControlData;
    if( buf == nullptr ) { //if new fails for some reason
        OPResult op_res(ERR_OUT_OF_MEMORY, LibnxError_OutOfMemory);
        writeToLog(op_res, LogWriter::WARNING);
        title_name = UNKNOWN_PARAMETER_STR;
        title_author = UNKNOWN_PARAMETER_STR;
        return; //we have to return and this condition is not an error, just a warning
    }

    memset(buf, 0, sizeof(NsApplicationControlData)); //setting the memory to 0 (as suggested in libnx examples)

    //initializing ns
    writeToLog("Initializing ns service");
    Result res = nsInitialize();
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_INITIALIZE_NS, R_DESCRIPTION(res));
        writeToLog(op_res, LogWriter::WARNING);
        delete buf;
        title_name = UNKNOWN_PARAMETER_STR;
        title_author = UNKNOWN_PARAMETER_STR;
        return; //we have to return and this condition is not an error, just a warning
    }

    writeToLog("Obtaining the actual savefile informations");
    res = nsGetApplicationControlData(1, title_id, buf, sizeof(NsApplicationControlData), &outsize);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_GET_APP_CONTROLDATA, R_DESCRIPTION(res));
        writeToLog(op_res, LogWriter::WARNING);
        delete buf;
        title_name = UNKNOWN_PARAMETER_STR;
        title_author = UNKNOWN_PARAMETER_STR;
        nsExit();
        return; //we have to return and this condition is not an error, just a warning
    }

    if( outsize < sizeof(buf->nacp) ) {
        OPResult op_res(ERR_OUTSIZE_TOO_SMALL);
        writeToLog(op_res, LogWriter::WARNING);
        delete buf;
        title_name = UNKNOWN_PARAMETER_STR;
        title_author = UNKNOWN_PARAMETER_STR;
        nsExit();
        return; //we have to return and this condition is not an error, just a warning
    }

    if( strncmp(buf->nacp.lang[langentry].name, "", sizeof(name)) == 0 ) {
        OPResult op_res(ERR_NS_NOT_FOUND);
        writeToLog(op_res, LogWriter::WARNING);
        delete buf;
        title_name = UNKNOWN_PARAMETER_STR;
        title_author = UNKNOWN_PARAMETER_STR;
        nsExit();
        return; //we have to return and this condition is not an error, just a warning
    }

    memset(name, 0, sizeof(name)); //do not assume it is null-terminated
    strncpy(name, buf->nacp.lang[langentry].name, sizeof(name)-1);
    title_name = name;

    if( strncmp(buf->nacp.lang[langentry].author, "", sizeof(author)) == 0 ) {
        OPResult op_res(ERR_NS_NOT_FOUND);
        writeToLog(op_res, LogWriter::WARNING);
        delete buf;
        title_name = UNKNOWN_PARAMETER_STR;
        title_author = UNKNOWN_PARAMETER_STR;
        nsExit();
        return; //we have to return and this condition is not an error, just a warning
    }

    memset(author, 0, sizeof(author)); //do not assume it is null-terminated
    strncpy(author, buf->nacp.lang[langentry].author, sizeof(author)-1);
    title_author = author;

    delete buf;
    nsExit();
    #else
    std::ostringstream log_str;
    log_str << "Getting additional information for " << folder_path;
    writeToLog(log_str.str());

    std::ostringstream path_stream;
    path_stream << folder_path << DEFAULT_SAVEHEADER_NAME;

    std::ostringstream log_str2;
    log_str2 << "Reading the header at " << path_stream.str();
    writeToLog(log_str2.str());

    std::ifstream file_stream(path_stream.str().c_str(), std::ios::in);
    if( !file_stream ) {
        OPResult op_res(ERR_OPEN_STREAM);
        writeToLog(op_res, LogWriter::WARNING);
        title_id = 0;
        title_name = UNKNOWN_PARAMETER_STR;
        title_author = UNKNOWN_PARAMETER_STR;
    }

    std::stringstream file_buffer;
    file_buffer << file_stream.rdbuf();

    getSaveFileInformationFromHeader(file_buffer);
    #endif

    std::ostringstream log_str_final;
    log_str_final << "Additional information found. " <<  "Title " << std::hex << title_id << " is " << title_name << ", " << title_author;
    writeToLog(log_str_final.str());
}

#ifndef EMULATOR
void SaveFile::getUserInformation() {
    //pretty much following the libnx's acc example
    AccountProfile profile;
    AccountUserData userdata;
    AccountProfileBase profilebase;

    char username[0x21];

    memset(&userdata, 0, sizeof(userdata));
    memset(&profilebase, 0, sizeof(profilebase));

    //retrieving information on username
    writeToLog("Retrieving account name informations");

    writeToLog("Initializing acc");
    Result res = accountInitialize();
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_INITIALIZE_ACC, R_DESCRIPTION(res));
        writeToLog(op_res, LogWriter::WARNING);
        account_name = UNKNOWN_PARAMETER_STR;
        return; //we have to return and this condition is not an error, just a warning
    }

    writeToLog("Get profile informations");
    res = accountGetProfile(&profile, user_id);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_GET_USER_PROFILE, R_DESCRIPTION(res));
        writeToLog(op_res, LogWriter::WARNING);
        account_name = UNKNOWN_PARAMETER_STR;
        accountExit();
        return; //we have to return and this condition is not an error, just a warning
    }

    res = accountProfileGet(&profile, &userdata, &profilebase);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_GET_USER_PROFILE, R_DESCRIPTION(res));
        writeToLog(op_res, LogWriter::WARNING);
        account_name = UNKNOWN_PARAMETER_STR;
        accountExit();
        return; //we have to return and this condition is not an error, just a warning
    }

    if( strncmp(profilebase.username, "", sizeof(username)) == 0 ) {
        OPResult op_res(ERR_GET_USER_PROFILE, R_DESCRIPTION(res));
        writeToLog(op_res, LogWriter::WARNING);
        account_name = UNKNOWN_PARAMETER_STR;
        accountExit();
        return; //we have to return and this condition is not an error, just a warning
    }

    memset(username,  0, sizeof(username));
    strncpy(username, profilebase.username, sizeof(username)-1);//Even though profilebase.username usually has a NUL-terminator, don't assume it does for safety.

    account_name = username;
    writeToLog("The account name is "+account_name);
}
#endif

#ifdef EMULATOR
void SaveFile::getSaveFileInformationFromHeader(std::stringstream& theStringStream) {
    std::size_t element_pos;
    std::size_t value_pos;
    std::size_t value_length;
    std::size_t newline_pos;

    //searching for the id in the header
    element_pos = theStringStream.str().find(HEADER_ID_STR);
    if( element_pos == std::string::npos ) {
        OPResult op_res(ERR_INVALID_SAVEHEADER);
        writeToLog(op_res, LogWriter::WARNING);
        title_id = 0;
    }

    newline_pos = theStringStream.str().find("\n", element_pos);
    value_pos = element_pos + HEADER_ID_STR.size() + HEADER_SEPARATOR.size();
    if( newline_pos == std::string::npos ) value_length = std::string::npos;
    else value_length = newline_pos - element_pos - HEADER_ID_STR.size() - HEADER_SEPARATOR.size() - 1;
    std::string temp = theStringStream.str().substr(value_pos, value_length);
    title_id = std::strtoul(temp.c_str(), nullptr, 10);

    //searching for the name in the header
    element_pos = theStringStream.str().find(HEADER_NAME_STR);
    if( element_pos != std::string::npos ) {
        newline_pos = theStringStream.str().find("\n", element_pos);
        value_pos = element_pos + HEADER_NAME_STR.size() + HEADER_SEPARATOR.size();
        if( newline_pos == std::string::npos ) value_length = std::string::npos;
        else value_length = newline_pos - element_pos - HEADER_NAME_STR.size() - HEADER_SEPARATOR.size() - 1;
        title_name = theStringStream.str().substr(value_pos, value_length);
    }

    else {
        OPResult op_res(ERR_INVALID_SAVEHEADER);
        writeToLog(op_res, LogWriter::WARNING);
        title_name = UNKNOWN_PARAMETER_STR;
    }

    //searching for the author in the header
    element_pos = theStringStream.str().find(HEADER_AUTHOR_STR);
    if( element_pos != std::string::npos ) {
        newline_pos = theStringStream.str().find("\n", element_pos);
        value_pos = element_pos + HEADER_AUTHOR_STR.size() + HEADER_SEPARATOR.size();
        if( newline_pos == std::string::npos ) value_length = std::string::npos;
        else value_length = newline_pos - element_pos - HEADER_AUTHOR_STR.size() - HEADER_SEPARATOR.size() - 1;
        title_author = theStringStream.str().substr(value_pos, value_length);
    }

    else {
        OPResult op_res(ERR_INVALID_SAVEHEADER);
        writeToLog(op_res, LogWriter::WARNING);
        title_author = UNKNOWN_PARAMETER_STR;
    }
}
#endif

std::vector<SaveFile> SaveFile::getAllSaveFiles() {
    std::vector<SaveFile> buffer;
    writeToLog("Starting a savefile scan on the system", 1);

    #ifdef EMULATOR
    writeToLog("Opening the iterator");
    DIR* d = opendir(DEFAULT_SAVEPATH.c_str()); // open the path
    if( d == NULL ) {
        OPResult op_res(ERR_OPEN_ITERATOR);
        writeToLog(op_res);
        return buffer;
    }

    dirent* dir; // for the directory entries

    while ((dir = readdir(d)) != NULL) { // if we were able to read somehting from the directory
        if(dir-> d_type == DT_DIR && strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            std::ostringstream log_str;
            log_str << "Found " << dir->d_name;
            writeToLog(log_str.str());

            std::ostringstream final_path;
            final_path << DEFAULT_SAVEPATH << dir->d_name;
            buffer.push_back(SaveFile(final_path.str()));
        }
    }

    closedir(d); // finally close the directory
    #else
    //some useful variables
    FsSaveDataIterator it;
    size_t total_entries = 0;
    size_t max_entries = 150; //this code will fail with more than 150 save file found
    FsSaveDataInfo info[max_entries];

    //opening the iterator to scan all the saves
    writeToLog("Opening the savefile iterator");
    Result res = fsOpenSaveDataIterator(&it, FsSaveDataSpaceId_NandUser);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_OPEN_ITERATOR, R_DESCRIPTION(res));
        writeToLog(op_res);
        return buffer;
    }

    //reading the iterator THIS PART OF THE CODE IS NOT READY AT ALL, I SHOULD ASK ABOUT A LOT OF OBSCURE THINGS
    res = fsSaveDataIteratorRead(&it, info, max_entries, &total_entries);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_OPEN_ITERATOR, R_DESCRIPTION(res));
        writeToLog(op_res);
        return buffer;
    }

    if( total_entries >= max_entries ) {
        OPResult op_res(ERR_TOO_MANY_SAVEFILES, R_DESCRIPTION(res));
        writeToLog(op_res);
        return buffer;
    }

    for( unsigned int i = 0; i < total_entries; i++ ) {
        if( info[i].SaveDataType == FsSaveDataType_SaveData ) {
            std::ostringstream log_str;
            log_str << "Found " << std::hex << info[i].titleID;
            writeToLog(log_str.str());
            buffer.push_back(SaveFile(info[i].titleID, info[i].userID));
        }
    }
    #endif
    std::ostringstream final_str;
    final_str << "Savefile scan on the system SUCCESS. Found " << buffer.size() << " savefiles";
    writeToLog("Savefile scan on the system SUCCESS");
    return buffer;
}

#ifndef EMULATOR
OPResult SaveFile::mount(const std::string& theMountName) {
    std::ostringstream initial_stream;
    initial_stream << "Mounting " << title_id << " for user " << std::hex << ((u64)(user_id>>64)) << "/" << std::hex << ((u64)(user_id)) << " to " << theMountName;
    writeToLog(initial_stream.str());

    FsFileSystem tmpfs;
    Result res = fsMount_SaveData(&tmpfs, title_id, user_id);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_MOUNT_SAVEDATA, R_DESCRIPTION(res));
        writeToLog(op_res);
        return op_res;
    }

    if( fsdevMountDevice(theMountName.c_str(), tmpfs) == -1 ) {
        OPResult op_res(ERR_MOUNT_DEVICE);
        writeToLog(op_res);
        return op_res;
    }

    mount_name = theMountName;
    return OPResult(OPResult::SUCCESS);
}
#endif

#ifndef EMULATOR
OPResult SaveFile::unmount() {
    std::ostringstream initial_stream;
    initial_stream << "Unmounting " << title_id << " for user " << std::hex << ((u64)(user_id>>64)) << "/" << std::hex << ((u64)(user_id)) << " from " << mount_name;
    writeToLog(initial_stream.str());

    if( fsdevUnmountDevice(mount_name.c_str()) == -1 ) {
        OPResult op_res(ERR_UNMOUNT_SAVEDATA);
        writeToLog(op_res);
        return op_res;
    }

    else {
        mount_name = "";
        return OPResult(OPResult::SUCCESS);
    }
}
#endif

#ifndef EMULATOR
OPResult SaveFile::commit() {
    writeToLog("committing "+mount_name);
    Result res = fsdevCommitDevice(mount_name.c_str());
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_COMMIT_SAVEDATA, R_DESCRIPTION(res));
        writeToLog(op_res);
        return op_res;
    }

    return OPResult(OPResult::SUCCESS);
}
#endif

//this function is an actual MESS because i am apparently unable to write a good recursive wipe function (don't bother with me, i am the worst)
OPResult SaveFile::wipePath(const std::string& thePath) {
    DIR* d = opendir(thePath.c_str()); // open the path
    if( d == NULL ) {
        OPResult op_res(ERR_OPEN_ITERATOR);
        writeToLog(op_res);
        return op_res;
    }

    dirent* dir; // for the directory entries

    OPResult op_res;
    while ((dir = readdir(d)) != NULL) {
        if( dir->d_type == DT_DIR ) {
            op_res = wipeFiles(thePath+"/"+dir->d_name);
            if( !op_res ) return op_res;

            op_res = wipeFolders(thePath+"/"+dir->d_name);
            if( !op_res ) return op_res;
        }

        if(dir->d_type == DT_REG) {
            #ifndef EMULATOR //yuzu currently crashes on remove
            writeToLog("Removing file "+thePath+"/"+std::string(dir->d_name));
            if( remove((thePath+"/"+std::string(dir->d_name)).c_str()) != 0 ) {
                OPResult op_res(ERR_DELETE_FILE);
                writeToLog(op_res);
                closedir(d);
                return op_res;

            }
            #else
            if( DEFAULT_SAVEHEADER_NAME != dir->d_name )
                writeToLog("Removing "+thePath+"/"+std::string(dir->d_name));
            #endif
        }
    }

    closedir(d);
    return OPResult(OPResult::SUCCESS);
}

OPResult SaveFile::wipeFiles(const std::string& thePath) {
    //just deleting files
    DIR* d = opendir(thePath.c_str()); // open the path
    if( d == NULL ) {
        OPResult op_res(ERR_OPEN_ITERATOR);
        writeToLog(op_res);
        return op_res;
    }

    dirent* dir; // for the directory entries

    //just deleting files
    OPResult op_res;
    while ((dir = readdir(d)) != NULL) {
        if( dir->d_type == DT_DIR )
            if( !(op_res = wipeFiles(thePath+"/"+std::string(dir->d_name))) )
                return op_res;

        if(dir->d_type == DT_REG) {
            #ifndef EMULATOR //yuzu currently crashes on remove
            writeToLog("Removing file "+thePath+"/"+std::string(dir->d_name));
            if( remove((thePath+"/"+std::string(dir->d_name)).c_str()) != 0 ) {
                OPResult op_res(ERR_DELETE_FILE);
                writeToLog(op_res);
                closedir(d);
                return op_res;

            }
            #else
            if( DEFAULT_SAVEHEADER_NAME != dir->d_name )
                writeToLog("Removing "+thePath+"/"+std::string(dir->d_name));
            #endif
        }
    }

    closedir(d);
    return OPResult(OPResult::SUCCESS);
}

OPResult SaveFile::wipeFolders(const std::string& thePath) {
    //just deleting folders
    DIR* d = opendir(thePath.c_str()); // open the path
    if( d == NULL ) { // if was not able return
        OPResult op_res(ERR_OPEN_ITERATOR);
        writeToLog(op_res);
        return op_res;
    }

    dirent* dir; // for the directory entries

    //just deleting folders
    Result op_res;

    while((dir = readdir(d)) != NULL) {
        if( dir->d_type == DT_DIR ) {
            if( !(op_res = wipeFolders(thePath+"/"+std::string(dir->d_name))) )
                return op_res;
        }
    }

    writeToLog("Removing folder "+thePath);
    #ifndef EMULATOR
    if( rmdir(thePath.c_str()) != 0 ) {
        OPResult op_res(ERR_DELETE_FOLDER);
        writeToLog(op_res);
        closedir(d);
        return op_res;
    }
    #endif
    closedir(d);
    return OPResult(OPResult::SUCCESS);
}

OPResult SaveFile::extractSVIToPath(const std::string& theSVIPath, const std::string& theDestinationPath) {
    std::ostringstream initial_stream;
    initial_stream << "Extracting " << theSVIPath << " to " << theDestinationPath;

    writeToLog("Initializing the archive");
    //initializing an archive
    mz_zip_archive archive;
    mz_zip_zero_struct(&archive);
    if( !mz_zip_reader_init_file(&archive, theSVIPath.c_str(), 0) ) {
        OPResult op_res(ERR_INIT_ARCHIVE);
        writeToLog(op_res);
        return op_res;
    }

    unsigned int file_num = mz_zip_reader_get_num_files(&archive);

    //scrolling through it
    for(unsigned int i = 0; i < file_num; i++) {
        mz_zip_reader_extract_iter_state* it = mz_zip_reader_extract_iter_new(&archive, i, 0); //creating the iterator
        const int MAX_FILE_SIZE = 2*10^7; //hoping that 20mb will always be enough (?)
        char* file_buffer = new char[MAX_FILE_SIZE];
        size_t file_size = mz_zip_reader_extract_iter_read(it, file_buffer, MAX_FILE_SIZE); //reading the file

        //if it is a directory we create it
        if( it->file_stat.m_is_directory ) {
            std::ostringstream dir_stream;
            dir_stream << "Creating " << theDestinationPath+it->file_stat.m_filename;
            writeToLog(dir_stream.str());

            if( mkdir((theDestinationPath+it->file_stat.m_filename).c_str(), 0x777) != 0 ) {
                OPResult op_res(ERR_CREATE_DIRECTORY);
                writeToLog(op_res);
                delete [] file_buffer;
                mz_zip_reader_extract_iter_free(it);
                return op_res;
            }
        }

        //if it is a file we write it to disk
        else {
            if( std::string(it->file_stat.m_filename) != DEFAULT_SAVEHEADER_NAME ) {
                writeToLog("Writing file "+theDestinationPath+it->file_stat.m_filename);
                std::ofstream stream((theDestinationPath+it->file_stat.m_filename).c_str(), std::ios::out | std::ios::binary);
                    if( !stream.is_open() ) {
                        OPResult op_res(ERR_OPEN_STREAM);
                        writeToLog(op_res);
                        delete [] file_buffer;
                        mz_zip_reader_extract_iter_free(it);
                        return op_res;
                    }

                    stream.write(file_buffer, file_size);
                    if( !stream.good() ) {
                        OPResult op_res(ERR_WRITE_FILE);
                        writeToLog(op_res);
                        delete [] file_buffer;
                        mz_zip_reader_extract_iter_free(it);
                        return op_res;
                    }
                }
            }

            delete [] file_buffer;
            mz_zip_reader_extract_iter_free(it);
        }

        mz_zip_end(&archive);
        return OPResult(OPResult::SUCCESS);
}


OPResult SaveFile::extractToSVIFile(const std::string& theSVIPath) {
    std::ostringstream initial_stream;
    initial_stream << "Starting the SVI extraction process for " << std::hex << title_id << " to " << theSVIPath;
    writeToLog(initial_stream.str(), 1);

    OPResult op_res;
    std::string base_path;

    #ifndef EMULATOR
    op_res = mount(DEFAULT_MOUNTNAME);
    if( !op_res ) return op_res;

    base_path = mount_name + ":/";
    #else
    base_path = folder_path;
    #endif

    char* archive_buffer; //here we will store the archive to write it on disk
    size_t archive_buffer_size;
    mz_zip_archive archive;
    mz_zip_zero_struct(&archive);

    std::string str_header = createSaveHeader().str();

    writeToLog("Initializing the archive");
    if( !mz_zip_writer_init_heap(&archive, 0, str_header.size()) ) {
        OPResult op_res(ERR_INIT_ARCHIVE);
        writeToLog(op_res);
        #ifndef EMULATOR
        unmount();
        #endif
        return op_res;
    }

    writeToLog("Writing the header into the archive");
    if( !mz_zip_writer_add_mem(&archive, DEFAULT_SAVEHEADER_NAME.c_str(), str_header.c_str(), str_header.size(), MZ_DEFAULT_COMPRESSION) ) {
        OPResult op_res(ERR_WRITE_ARCHIVE_HEADER);
        writeToLog(op_res);
        #ifndef EMULATOR
        unmount();
        #endif
        mz_zip_end(&archive);
        return op_res;
    }

    op_res = extractPathToSVI(archive, base_path);
    if( !op_res ) {
        #ifndef EMULATOR
        unmount();
        #endif
        mz_zip_end(&archive);
        return op_res;
    }

    writeToLog("Finalizing archive");
    if( !mz_zip_writer_finalize_heap_archive(&archive, ((void**)(&archive_buffer)), &archive_buffer_size) ) {
        OPResult op_res(ERR_FINALIZE_ARCHIVE);
        writeToLog(op_res);
        #ifndef EMULATOR
        unmount();
        #endif
        mz_zip_end(&archive);
        return op_res;
    }

    writeToLog("Writing the file on disk");
    std::ofstream stream(theSVIPath.c_str(), std::ios::out | std::ios::binary);
    if( !stream.is_open() ) {
        OPResult op_res(ERR_OPEN_STREAM);
        delete [] archive_buffer;
        #ifndef EMULATOR
        unmount();
        #endif
        mz_zip_end(&archive);
        return op_res;
    }

    stream.write(archive_buffer, archive_buffer_size);
    if( !stream.good() ) {
        OPResult op_res(ERR_WRITE_FILE);
        delete [] archive_buffer;
        #ifndef EMULATOR
        unmount();
        #endif
        mz_zip_end(&archive);
        return op_res;
    }

    delete [] archive_buffer;
    mz_zip_end(&archive);

    #ifndef EMULATOR
    op_res = unmount();
    if( !op_res ) return op_res;
    #endif

    writeToLog("SVI extraction process SUCCESS");
    return OPResult(OPResult::SUCCESS);
}

std::ostringstream SaveFile::createSaveHeader() const {
    std::ostringstream buffer;

    buffer <<   HEADER_ID_STR << HEADER_SEPARATOR << title_id << "\n" <<
                HEADER_NAME_STR << HEADER_SEPARATOR << title_name << "\n" <<
                HEADER_AUTHOR_STR << HEADER_SEPARATOR << title_author << "\n";

    return buffer;
}

OPResult SaveFile::extractPathToSVI(mz_zip_archive& theArchive, const std::string& theSourcePath, const std::string& theIterator) const {
    std::ostringstream initial_stream;
    initial_stream << "Exporting " << theSourcePath << " into the archive";
    writeToLog(initial_stream.str());

    DIR* d = opendir(theSourcePath.c_str()); // open the path
    if( d == NULL ) {
        OPResult op_res(ERR_READ_ITERATOR);
        writeToLog(op_res);
        return op_res;
    } // if was not able return

    dirent* dir; // for the directory entries
    char fake_buffer; //i could probably pass nullptr but who knows...
    OPResult res;

    while((dir = readdir(d)) != NULL) {
        if( dir->d_type == DT_DIR ) {
            std::ostringstream dir_stream;
            dir_stream << "Creating directory " << theIterator << dir->d_name << "/" << " in the archive";
            writeToLog(dir_stream.str());

            if( !mz_zip_writer_add_mem(&theArchive, (theIterator+std::string(dir->d_name)+"/").c_str(), &fake_buffer, 0, MZ_DEFAULT_COMPRESSION) ) {
                OPResult op_res(ERR_CREATE_ARCHIVE_DIRECTORY);
                writeToLog(op_res);
                return op_res;
            }

            if( !(res = extractPathToSVI(theArchive, theSourcePath+std::string(dir->d_name)+"/", theIterator+std::string(dir->d_name)+"/")) ) return res;
        }

        else if( dir->d_type == DT_REG ) {
            #ifdef EMULATOR
            if( dir->d_name != DEFAULT_SAVEHEADER_NAME ) {
            #endif // EMULATOR
            std::ostringstream file_stream;
            file_stream << "Creating file " << theIterator << dir->d_name << " in the archive";
            writeToLog(file_stream.str());
            if( !mz_zip_writer_add_file(&theArchive, (theIterator+dir->d_name).c_str(), (theSourcePath+dir->d_name).c_str(), nullptr, 0, MZ_DEFAULT_COMPRESSION) ) return OPResult(ERR_WRITE_ARCHIVE_FILE);
            #ifdef EMULATOR
            }
            #endif
        }
    }

    closedir(d);
    return OPResult(OPResult::SUCCESS);
}

OPResult SaveFile::importFromSVIFile(const SVIFile& theSVIFile) {
    std::ostringstream initial_stream;
    initial_stream << "Starting an import operation for " << std::hex << title_id << " from " << theSVIFile.getPath();
    writeToLog(initial_stream.str(), 1);

    OPResult op_res;

    //just a fast check on the SVI file
    if( !theSVIFile.isValid() ) {
        OPResult op_res(ERR_INVALID_SVI);
        writeToLog(op_res);
        return op_res;
    }

    #ifndef EMULATOR
    //mounting the savefile
    op_res = mount(DEFAULT_MOUNTNAME);
    if( !op_res ) return op_res;
    #endif

    std::string mount_point;
    #ifndef EMULATOR
    mount_point = mount_name + ":/";
    #else
    mount_point = folder_path;
    #endif

    //wiping the save out
    op_res = wipePath(mount_point);
    if( !op_res ) {
        #ifndef EMULATOR
        unmount();
        #endif
        return op_res;
    }

    //actually importing the save
    op_res = extractSVIToPath(theSVIFile.getPath(), mount_point);
    if( !op_res ) {
        #ifndef EMULATOR
        unmount();
        #endif
        return op_res;
    }

    #ifndef EMULATOR
    //committing the save
    op_res = commit();
    if( !op_res ) {
        unmount();
        return op_res;
    }

    //unmounting the save
    op_res = unmount();
    if( !op_res ) {
        unmount();
        return op_res;
    }
    #endif

    writeToLog("Import operation SUCCESS");
    return OPResult(OPResult::SUCCESS);
}
