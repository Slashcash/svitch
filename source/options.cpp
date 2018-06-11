#ifndef _OPTIONS_CPP_
#define _OPTIONS_CPP_

#include <string>

#include <switch/types.h>

const std::string BASE_PATH;
#ifndef EMULATOR
const std::string ROMFS_PATH = "romfs:/";
#else
const std::string ROMFS_PATH = BASE_PATH + "romfs/";
#endif
const std::string FONT_PATH = ROMFS_PATH + "fonts/";
const std::string TEXTURE_PATH = ROMFS_PATH + "textures/";

const std::string EXPORT_PATH = BASE_PATH + "saves/";
const std::string IMPORT_PATH = BASE_PATH + "saves/";
const std::string BACKUP_PATH = BASE_PATH + IMPORT_PATH + "backup/";

const std::string APP_TITLE = "Svitch";
const std::string APP_VERSION = "v0.2.1-alpha";
const std::string APP = APP_TITLE + " " + APP_VERSION;

const std::string DEFAULT_LANGUAGECODE = "en-GB";
const s32 DEFAULT_LANGUAGE = 12;
#ifdef EMULATOR
const std::string EMULATOR_LANGUAGECODE = "en-GB";
const s32 EMULATOR_LANGUAGE = 12;
#endif

#endif // _OPTIONS_CPP_
