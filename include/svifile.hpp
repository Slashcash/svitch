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
        std::string title_name; //the game title for this savefile
        std::string title_author; //the game author of this savefile
        bool is_valid;

        void getInformations();

    public:
        SVIFile() = delete;
        SVIFile(const std::string& thePath);

        std::string getPath() const { return file_path; }
        bool isValid() const { return is_valid; }
        u64 getTitleID() const { return title_id; }
        std::string getTitleName() const { return title_name; }
        std::string getTitleAuthor() const { return title_author; }

        static std::vector<SVIFile> getAllSVIInPath(const std::string& thePath);
};

#endif // _SVIFILE_HPP_
