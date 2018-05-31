#include "logwriter.hpp"

#include <sstream>
#include <iostream>

#include "options.cpp"

bool LogWriter::is_initialized = false;
LogWriter::Level LogWriter::log_level = LogWriter::Level::DEBUG;
bool LogWriter::is_active = true;
std::ofstream LogWriter::log;
std::string LogWriter::level_str[LogWriter::Level::ENUM_COUNT];

const std::string LogWriter::DEBUG_STR = "DEBUG";
const std::string LogWriter::WARNING_STR = "WARNING";
const std::string LogWriter::ERROR_STR = "ERROR";

void LogWriter::initialize() {
        level_str[Level::DEBUG] = DEBUG_STR;
        level_str[Level::WARNING] = WARNING_STR;
        level_str[Level::ERROR] = ERROR_STR;

        std::ostringstream log_path;
        log_path << BASE_PATH << "/log.txt";

        log.open(log_path.str().c_str(), std::ios::out); //opening the stream

        //if opening is succesful we redirect the log to std::cerr
        if( log.is_open() && log.good() ) std::cerr.rdbuf(log.rdbuf());

        //the initialization is considered valid anyway
        is_initialized = true;
}

void LogWriter::writeToLog(const std::string& theMSG, const Level theLevel) {
    if( !is_initialized ) initialize();

    if( is_active ) {
        if( theLevel >= log_level ) {
            std::ostringstream msg;
            msg << level_str[theLevel] << ": " << theMSG << "\n";
            std::cerr << msg.str() << std::flush;
        }
    }
}

void LogWriter::writeToLog(const OPResult& theResult, const Level theLevel) {
    std::ostringstream msg;
    msg << "Failed with error " << theResult.str();

    writeToLog(msg.str(), theLevel);
}
