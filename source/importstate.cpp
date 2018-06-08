#include "importstate.hpp"

#include "options.cpp"
#include "resultstate.hpp"
#include "langfile.hpp"
#include "gui.hpp"

ImportState::ImportState(const SaveFile& theSaveFile) : savefile(theSaveFile) {
    //variable initialization
    save_selected = 0;

    //texts initialization
    no_importing.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    no_importing.setCharacterSize(20);
    no_importing.setMsg(LangFile::getInstance()->getValue("no_importing")+" "+savefile.getBestSuitableName(Gui::getInstance()->getSystemLanguage()));
    no_importing.setPosition( (Window::getInstance()->getSize().x / 2) - (no_importing.getSize().x /2), 500 );

    importing_for.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    importing_for.setCharacterSize(20);
    importing_for.setMsg(LangFile::getInstance()->getValue("importing_for")+" "+savefile.getBestSuitableName(Gui::getInstance()->getSystemLanguage()));
    importing_for.setPosition( (Window::getInstance()->getSize().x / 2) - (importing_for.getSize().x /2), 150 );

    file_found.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    file_found.setCharacterSize(25);

    file_info.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    file_info.setCharacterSize(25);

    game_author.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    game_author.setCharacterSize(35);

    game_title.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    game_title.setCharacterSize(35);
    game_title.attachChild(&game_author);
    game_title.attachChild(&file_info);
    game_title.attachChild(&file_found);

    scene.addToLayer(&game_title, 0);

    arrow_info.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    arrow_info.setMsg(LangFile::getInstance()->getValue("press_arrow_scroll_files"));
    arrow_info.setPosition(100, 600);

    import_info.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    import_info.setMsg(LangFile::getInstance()->getValue("press_a_import"));
    arrow_info.attachChild(&import_info);
    import_info.setPosition(0, arrow_info.getSize().y + 10);

    exit_info.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    exit_info.setMsg(LangFile::getInstance()->getValue("press_b_undo"));
    import_info.attachChild(&exit_info);
    exit_info.setPosition(0, import_info.getSize().y + 10);

    //setting the inputs
    std::vector<InputSignal> signals;
    InputEvent event_b(InputEvent::BUTTON_PRESSED, KEY_B);
    Signal signal_b("UNDO");
    signals.push_back(std::make_pair(event_b, signal_b));

    InputEvent event_a(InputEvent::BUTTON_PRESSED, KEY_A);
    Signal signal_a("IMPORT");
    signals.push_back(std::make_pair(event_a, signal_a));

    #ifndef EMULATOR
    InputEvent event_l(InputEvent::BUTTON_PRESSED, BUTTON_DLEFT);
    #else
    InputEvent event_l(InputEvent::BUTTON_PRESSED, BUTTON_MINUS);
    #endif
    Signal signal_l("LEFT");
    signals.push_back(std::make_pair(event_l, signal_l));

    #ifndef EMULATOR
    InputEvent event_r(InputEvent::BUTTON_PRESSED, BUTTON_DRIGHT);
    #else
    InputEvent event_r(InputEvent::BUTTON_PRESSED, BUTTON_PLUS);
    #endif
    Signal signal_r("RIGHT");
    signals.push_back(std::make_pair(event_r, signal_r));

    setInputSignal(signals);

    //getting all the savefiles
    savefiles_folder = SVIFile::getAllSVIInPath(IMPORT_PATH);

    //loading all the icons
    for( auto it = savefiles_folder.begin(); it < savefiles_folder.end(); it++ ) {
        std::ostringstream id;
        id << it->getTitleID();
        std::string temp_icon = it->getIcon();
        texture_manager.addResource((void*)(&(temp_icon[0])), temp_icon.size(), id.str());
    }

    //writing the name of first save on screen
    if( savefiles_folder.empty() ) {
        game_title.setMsg(LangFile::getInstance()->getValue("no_save") + " in " + IMPORT_PATH);
        game_title.setPosition( (Window::getInstance()->getSize().x / 2) - (game_title.getSize().x /2), (Window::getInstance()->getSize().y / 2) - (game_title.getSize().y / 2) );

        game_author.setMsg(LangFile::getInstance()->getValue("press_b_undo"));
        game_author.setPosition( (game_title.getSize().x / 2) - (game_author.getSize().x / 2), game_title.getSize().y + 10 );
    }

    else {
        buildTitleInfo();
        scene.addToLayer(&arrow_info, 0);
        scene.addToLayer(&importing_for, 0);
    }
}

void ImportState::buildTitleInfo() {
    scene.detachFromLayer(&no_importing, 0);

    game_title.setMsg(savefiles_folder[save_selected].getBestSuitableName(Gui::getInstance()->getSystemLanguage()));
    game_title.setPosition( (Window::getInstance()->getSize().x / 2) - (game_title.getSize().x /2), (Window::getInstance()->getSize().y / 2) - (game_title.getSize().y / 2) );

    game_author.setMsg(savefiles_folder[save_selected].getBestSuitableAuthor(Gui::getInstance()->getSystemLanguage()));
    game_author.setPosition( (game_title.getSize().x / 2) - (game_author.getSize().x / 2), game_title.getSize().y + 10 );

    file_info.setMsg(savefiles_folder[save_selected].getPath());
    file_info.setPosition( (game_title.getSize().x / 2) - (file_info.getSize().x / 2), -file_info.getSize().y - 20  );

    file_found.setMsg(LangFile::getInstance()->getValue("file_found"));
    file_found.setPosition( (game_title.getSize().x / 2) - (file_found.getSize().x / 2), file_info.getPosition().y -file_found.getSize().y - 25  );

    if( savefiles_folder[save_selected].getTitleID() != savefile.getTitleID() ) scene.addToLayer(&no_importing, 0);
}

void ImportState::onNotify(const Signal& theSignal) {
    if( theSignal.getName() == "UNDO" ) requestToExit();

    if( theSignal.getName() == "RIGHT" && !savefiles_folder.empty() && save_selected < savefiles_folder.size() - 1 ) emitSignal(Signal("SCROLL_RIGHT"));
    if( theSignal.getName() == "SCROLL_RIGHT" ) { save_selected++; buildTitleInfo(); }

    if( theSignal.getName() == "LEFT" && !savefiles_folder.empty() && save_selected > 0 ) emitSignal(Signal("SCROLL_LEFT"));
    if( theSignal.getName() == "SCROLL_LEFT" ) { save_selected--; buildTitleInfo(); }

    if( theSignal.getName() == "IMPORT" && !savefiles_folder.empty() && savefiles_folder[save_selected].getTitleID() == savefile.getTitleID() ) emitSignal(Signal("IMPORT_POSSIBLE"));
    if( theSignal.getName() == "IMPORT_POSSIBLE" ) import_svi();
}

void ImportState::import_svi() {
    char temp_buffer[256];

    std::string backup_path = BACKUP_PATH + itoa(savefiles_folder[save_selected].getTitleID(), temp_buffer, 16) + std::string(".svi");

    //attempting a desperate last minute backup of the save just in case something goes wrong
    OPResult res = savefile.extractToSVIFile(backup_path);

    res = savefile.importFromSVIFile(savefiles_folder[save_selected].getPath());
    std::string result_str;
    if( res ) result_str = LangFile::getInstance()->getValue("importing_succesful");
    else result_str = LangFile::getInstance()->getValue("importing_failed") + " " + std::string(itoa(res.getErrorNumber(), temp_buffer, 10)) + "-" + std::string(itoa(res.getLibNXErrorNumber(), temp_buffer, 10));

    requestToExit();
    ResultState* res_state = new ResultState(result_str);
    Gui::getInstance()->addState(res_state);
}
