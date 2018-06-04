#ifndef _SVIFILE_HPP_
#define _SVIFILE_HPP_

#include <string>
#include <switch/types.h>
#include <vector>

#include "opresult.hpp"
#include "logwriter.hpp"

class SVIFile : public LogWriter {
    private:
        std::string file_path;
        u64 title_id; //the title id for this savefile
        std::vector<std::string> title_names; //the game title for this savefile
        std::vector<std::string> title_authors; //the game author of this savefile
        bool is_valid;

        void getInformations();

    public:
        SVIFile() = delete;
        SVIFile(const std::string& thePath);

        std::string getPath() const { return file_path; }
        bool isValid() const { return is_valid; }
        u64 getTitleID() const { return title_id; }
        std::vector<std::string> getTitleNames() const { return title_names; }
        std::vector<std::string> getTitleAuthors() const { return title_authors; }
        std::string getBestSuitableName(const s32 theLanguage) const;
        std::string getBestSuitableAuthor(const s32 theLanguage) const; //these two functions return the name for the specified language, or the closest one if that is not available

        static std::vector<SVIFile> getAllSVIInPath(const std::string& thePath);
};

#endif // _SVIFILE_HPP_
