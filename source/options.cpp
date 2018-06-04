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
const std::string EXPORT_PATH = BASE_PATH + "saves/";
const std::string IMPORT_PATH = BASE_PATH + "saves/";
const std::string BACKUP_PATH = BASE_PATH + IMPORT_PATH + "backup/";

const std::string APP = "Svitch";

const std::string DEFAULT_LANGUAGECODE = "en-US";
const s32 DEFAULT_LANGUAGE = 1;

#endif // _OPTIONS_CPP_
