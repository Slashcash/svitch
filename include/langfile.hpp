#ifndef _LANGFILE_HPP_
#define _LANGFILE_HPP_

#include <map>

#include "opresult.hpp"
#include "logwriter.hpp"

/*A class that will store all the external strings for an application*/

class LangFile : public LogWriter {
    private:
        static const std::string SEPARATOR;
        static LangFile* instance;
        static bool is_initialized;

        std::map<const std::string, std::string> lang_map;

        LangFile() = default;
        ~LangFile() {}

    public:
        LangFile(const LangFile& theLangFile) = delete;
        LangFile& operator=(const LangFile& theLangFile) = delete;

        OPResult loadFromFile(const std::string& thePath);
        std::string getValue(const std::string& theKey) const;
        static LangFile* getInstance();
};

#endif // _LANGFILE_HPP_
