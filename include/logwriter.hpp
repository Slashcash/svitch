#ifndef _LOGWRITER_HPP_
#define _LOGWRITER_HPP_

#include <fstream>
#include <switch.h>

#include "opresult.hpp"

class LogWriter {
    protected:
        enum Level {
            DEBUG = 0,
            WARNING,
            ERROR,
            ENUM_COUNT
        };

    private:
        static const std::string DEBUG_STR;
        static const std::string WARNING_STR;
        static const std::string ERROR_STR;

        static bool is_initialized; //whether or not the log is initialized
        static bool is_active; //whether log is active or not
        static Level log_level; //this will filter the log out
        static std::ofstream log; //the actual log
        static std::string level_str[ENUM_COUNT];
        static Mutex mutex; //this is for thread safety

        static void initialize();

    protected:
        static Level getLevel() { return log_level; }
        static bool isActive() { return is_active; }

        static void setLevel(const Level theLevel) { log_level = theLevel; }
        static void setActive(const bool isActive) { is_active = isActive; }

        static void writeToLog(const std::string& theMSG, const unsigned int theTabNum = 0, const Level theLevel = DEBUG); //by passing a value > 0 as theTabNum the log message is written with  theTabNum tabs for readibility reasons
        static void writeToLog(const OPResult& theResult, const unsigned int theTabNum = 0, const Level theLevel = ERROR);
};

#endif // _LOGWRITER_HPP_
