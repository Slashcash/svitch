#include "gui.hpp"

#include <switch.h>
#include <algorithm>
#include <sys/stat.h>

#include "state.hpp"
#include "langfile.hpp"
#include "options.cpp"
#include "mainstate.hpp"

Gui* Gui::instance = nullptr;
bool Gui::is_initialized = false;

Gui::Gui() {
    mkdir(EXPORT_PATH.c_str(), 0x777);
    mkdir(IMPORT_PATH.c_str(), 0x777);
    mkdir(BACKUP_PATH.c_str(), 0x777);

    //creating the window
    if( !Window::getInstance()->isValid() ) return;

    #ifndef EMULATOR
    //enabling romfs
    writeToLog("Mounting romfs", 1);
    Result res = romfsInit();
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_MOUNT_ROMFS, R_DESCRIPTION(res));
        writeToLog(op_res);
        return;
    }

    writeToLog("Mounting romfs SUCCESS");
    #endif

    //all the language supported by this software (add them here to support more of them)
    supported_languages.push_back("en-US");
    supported_languages.push_back("en-GB");
    supported_languages.push_back("it");

    #ifndef EMULATOR
    //getting the system language
    getSetLanguage();

    //converting it into its numerical id
    setMakeLanguage(*((u64*)(system_language_code.c_str())), &system_language);

    //exiting set
    setExit();
    #else
    system_language = DEFAULT_LANGUAGE;
    system_language_code = DEFAULT_LANGUAGECODE;
    #endif

    std::ostringstream lang_stream;
    lang_stream << "System language is "  << system_language << " (" << system_language_code << ")";
    writeToLog(lang_stream.str());

    //loading the language file
    //seeing if we support the system language (switching back to default if we don't
    std::string lang_filename;
    if( std::find(supported_languages.begin(), supported_languages.end(), system_language_code) == supported_languages.end() ) lang_filename = DEFAULT_LANGUAGECODE + ".lang";
    else lang_filename = system_language_code + ".lang";

    if( !LangFile::getInstance()->loadFromFile(ROMFS_PATH+"lang/"+lang_filename) ) return;

    //adding the state
    addState(new MainState());
}

void Gui::getSetLanguage() {
    writeToLog("Scanning for the system language", 1);

    writeToLog("Initializing set");
    Result res = setInitialize();
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_GET_SYSTEM_LANGUAGE, R_DESCRIPTION(res));
        writeToLog(op_res, 0, LogWriter::WARNING);
        system_language_code = DEFAULT_LANGUAGECODE;
        return;
    }

    writeToLog("Actually getting the language");
    u64 temp_buffer;
    res = setGetSystemLanguage(&temp_buffer);
    if( R_FAILED(res) ) {
        OPResult op_res(ERR_GET_SYSTEM_LANGUAGE, R_DESCRIPTION(res));
        writeToLog(op_res, 0, LogWriter::WARNING);
        system_language_code = DEFAULT_LANGUAGECODE;
        return;
    }

    system_language_code = (char*)(&temp_buffer);
}

void Gui::run() {
    while( Window::getInstance()->isOpen() && !states.empty() ) {
        //updating
        states.top()->updateBase();

        //checking if the current state requested an exit
        if( states.top()->isRequestedToExit() ) { dropState(); continue; }

        //drawing
        Window::getInstance()->clear();
        states.top()->drawBase();
        Window::getInstance()->update();


    }
}

void Gui::dropState() {
    delete states.top();
    states.pop();
}

Gui* Gui::getInstance() {
    if( !is_initialized ) {
        instance = new Gui();
        is_initialized = true;
    }

    return instance;
}

void Gui::destroy() {
    //dropping out all the left out states
    while( !Gui::getInstance()->states.empty() ) Gui::getInstance()->dropState();
    delete instance;
    instance = nullptr;
    is_initialized = false;

    //destroying the langfile singleton
    LangFile::destroy();

    #ifndef EMULATOR
    //shutting down romfs
    romfsExit();
    #endif

    //destroying window
    Window::getInstance()->destroy();
}
