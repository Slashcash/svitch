#ifndef _SAVEFILE_HPP_
#define _SAVEFILE_HPP_

#include <string>
#include <vector>
#include <switch/types.h>
#include <miniz.h>

#include "logwriter.hpp"
#include "svifile.hpp"

#ifndef EMULATOR
struct Account { //all the account on the system
    std::string name;
    std::string icon;
    std::size_t icon_size;
};
#endif

class SaveFile : public LogWriter {
    private:
        friend class SVIFile;

        #ifdef EMULATOR
        static const std::string DEFAULT_SAVEPATH;
        #else
        static const std::string DEFAULT_MOUNTNAME;
        #endif
        static const std::string DEFAULT_SAVEHEADER_NAME;
        static const std::string DEFAULT_ICON_NAME;
        static const std::string HEADER_SEPARATOR;
        static const std::string UNKNOWN_PARAMETER_STR;
        static const std::string HEADER_ID_STR;
        static const std::string HEADER_NAME_STR;
        static const std::string HEADER_AUTHOR_STR;
        static const std::string HEADER_EXTRACTED_STR;

        #ifdef EMULATOR
        std::string folder_path;
        #else
        u128 user_id;
        std::string mount_name;
        std::string account_name;
        static std::vector<Account> accounts; //all the accounts which at least have one save found on the system, gets filled after a succesful call of getAllSaveFiles
        #endif
        u64 title_id; //the title id for this savefile
        std::vector<std::string> title_names; //the game title for this savefile
        std::vector<std::string> title_authors; //the game author of this savefile
        std::string icon; //here we will store the icon as binary data (in jpeg) we store a copy of it in the texturemanager so it is probably overkill, i could probably encode back that copy and delete this but for now it's here
        static const unsigned int LANGUAGE_NUM = 16;

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
        OPResult commit();
        void getUserInformation();
        #endif
        void getSaveFileInformation(); //this retrieves the savefile name and author from system
        std::ostringstream createSaveHeader() const;
        OPResult wipePath(const std::string& thePath);
        OPResult wipeFiles(const std::string& thePath);
        OPResult wipeFolders(const std::string& thePath);
        OPResult extractPathToSVI(mz_zip_archive& theArchive, const std::string& theSourcePath, const std::string& theIterator = "") const;
        OPResult extractSVIToPath(const std::string& theSVIPath, const std::string& theDestinationPath);


    public:
        u64 getTitleID() const { return title_id; }
        std::vector<std::string> getTitleNames() const { return title_names; }
        std::vector<std::string> getTitleAuthors() const { return title_authors; }
        std::string getBestSuitableName(const s32 theLanguage) const;
        std::string getBestSuitableAuthor(const s32 theLanguage) const; //these two functions return the name for the specified language, or the closest one if that is not available
        std::string getIcon() const { return icon; }
        #ifndef EMULATOR
        u128 getUserID() const { return user_id; }
        std::string getUserName() const { return account_name; }
        static std::vector<Account> getAccounts() { return accounts; }
        #endif

        static std::vector<SaveFile> getAllSaveFiles(); //gets all the savefiles found on the system, theAccountBuffer gets filled with all the account on the system
        OPResult extractToSVIFile(const std::string& theSVIPath);
        OPResult importFromSVIFile(const SVIFile& theSVIFile);
};

#endif // _SAVEFILE_HPP_
