#ifndef _SAVEFILE_HPP_
#define _SAVEFILE_HPP_

#include <string>
#include <vector>
#include <switch/types.h>
#include <miniz.h>

#include "logwriter.hpp"
#include "svifile.hpp"

class SaveFile : public LogWriter {
    private:
        friend class SVIFile;

        #ifdef EMULATOR
        static const std::string DEFAULT_SAVEPATH;
        #else
        static const std::string DEFAULT_MOUNTNAME;
        #endif
        static const std::string DEFAULT_SAVEHEADER_NAME;
        static const std::string HEADER_SEPARATOR;
        static const std::string UNKNOWN_PARAMETER_STR;
        static const std::string HEADER_ID_STR;
        static const std::string HEADER_NAME_STR;
        static const std::string HEADER_AUTHOR_STR;

        #ifdef EMULATOR
        std::string folder_path;
        #else
        u128 user_id;
        std::string mount_name;
        #endif
        u64 title_id; //the title id for this savefile
        std::string title_name; //the game title for this savefile
        std::string title_author; //the game author of this savefile

        #ifdef EMULATOR
        SaveFile(const std::string& thePath);
        #else
        SaveFile(const u64 theTitleID, const u128 theUserID);
        #endif
        SaveFile() = delete;


        #ifdef EMULATOR
        void getSaveFileInformationFromHeader(std::stringstream& theStringStream); //this retrieves them from an header
        #else
        OPResult mount(const std::string& theMountName);
        OPResult unmount();
        #endif
        void getSaveFileInformation(); //this retrieves the savefile name and author from system
        std::ostringstream createSaveHeader() const;
        OPResult extractPathToSVI(mz_zip_archive& theArchive, const std::string& theSourcePath, const std::string& theIterator = "") const;


        public:
            u64 getTitleID() const { return title_id; }
            std::string getTitleName() const { return title_name; }
            std::string getTitleAuthor() const { return title_author; }

            static std::vector<SaveFile> getAllSaveFiles(); //gets all the savefiles found on the system
            OPResult extractToSVIFile(const std::string& theSVIPath);
};

#endif // _SAVEFILE_HPP_
