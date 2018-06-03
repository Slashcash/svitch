#include "mainstate.hpp"

#include <sys/stat.h>
#include <stdlib.h>

#include "gui.hpp"
#include "savefile.hpp"
#include "options.cpp"
#include "langfile.hpp"
#include "resultstate.hpp"
//#include "importstate.hpp"

MainState::MainState() {
    //creating the dirs
    mkdir(EXPORT_PATH.c_str(), 0x777);

    //variable initialization
    save_selected = 0;

    //texts initialization
    game_userid.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    game_title.setCharacterSize(25);
    game_author.attachChild(&game_userid);

    game_author.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    game_author.setCharacterSize(35);

    game_title.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    game_title.setCharacterSize(35);
    game_title.attachChild(&game_author);

    scene.addToLayer(&game_title, 0);

    arrow_info.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    arrow_info.setMsg(LangFile::getInstance()->getValue("press_arrow_scroll"));
    arrow_info.setPosition(100, 600);

    export_info.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    export_info.setMsg(LangFile::getInstance()->getValue("press_y_export"));
    arrow_info.attachChild(&export_info);
    export_info.setPosition(0, arrow_info.getSize().y + 10);

    import_info.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    import_info.setMsg(LangFile::getInstance()->getValue("press_x_import"));
    //export_info.attachChild(&import_info); //decomment this out when implementing the import function
    import_info.setPosition(0, export_info.getSize().y + 10);

    exit_info.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    exit_info.setMsg(LangFile::getInstance()->getValue("press_b_exit"));
    //import_info.attachChild(&exit_info);
    export_info.attachChild(&exit_info); //comment this out and decomment the line before this when implementing the import function
    exit_info.setPosition(0, import_info.getSize().y + 10);

    app_title.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    app_title.setMsg(APP);
    app_title.setPosition( (Window::getInstance()->getSize().x / 2) - (app_title.getSize().x /2), 25 );
    scene.addToLayer(&app_title, 0);

    //setting the inputs
    std::vector<InputSignal> signals;
    InputEvent event_b(InputEvent::BUTTON_PRESSED, KEY_B);
    Signal signal_b("EXIT");
    signals.push_back(std::make_pair(event_b, signal_b));

    InputEvent event_x(InputEvent::BUTTON_PRESSED, KEY_X);
    Signal signal_x("IMPORT");
    signals.push_back(std::make_pair(event_x, signal_x));

    InputEvent event_y(InputEvent::BUTTON_PRESSED, KEY_Y);
    Signal signal_y("EXPORT");
    signals.push_back(std::make_pair(event_y, signal_y));

    InputEvent event_l(InputEvent::BUTTON_PRESSED, KEY_MINUS);
    Signal signal_l("LEFT");
    signals.push_back(std::make_pair(event_l, signal_l));

    InputEvent event_r(InputEvent::BUTTON_PRESSED, KEY_PLUS);
    Signal signal_r("RIGHT");
    signals.push_back(std::make_pair(event_r, signal_r));

    setInputSignal(signals);

    //getting all the savefiles
    savefiles = SaveFile::getAllSaveFiles();

    //writing the name of first save on screen
    if( savefiles.empty() ) {
        game_title.setMsg(LangFile::getInstance()->getValue("no_save"));
        game_title.setPosition( (Window::getInstance()->getSize().x / 2) - (game_title.getSize().x /2), (Window::getInstance()->getSize().y / 2) - (game_title.getSize().y / 2) );

        game_author.setMsg(LangFile::getInstance()->getValue("press_b_exit"));
        game_author.setPosition( (game_title.getSize().x / 2) - (game_author.getSize().x / 2), game_title.getSize().y + 10 );
    }

    else {
        buildTitleInfo();
        scene.addToLayer(&arrow_info, 0);
    }
}

void MainState::buildTitleInfo() {
    const unsigned int SPACING = 10;

    game_title.setMsg(savefiles[save_selected].getTitleName());
    game_title.setPosition( (Window::getInstance()->getSize().x / 2) - (game_title.getSize().x /2), (Window::getInstance()->getSize().y / 2) - (game_title.getSize().y / 2) );

    game_author.setMsg(savefiles[save_selected].getTitleAuthor());
    game_author.setPosition( (game_title.getSize().x / 2) - (game_author.getSize().x / 2), game_title.getSize().y + SPACING );

    std::string user_id_temp_str;
    #ifdef EMULATOR
    u128 temp_user_id_buffer = 69696969696969; //a random number really
    std::ostringstream user_id_stream;
    user_id_stream << LangFile::getInstance()->getValue("user") << ": " << std::hex << ((u64)(temp_user_id_buffer>>64)) << "/" << std::hex << ((u64)(temp_user_id_buffer));
    user_id_temp_str = user_id_stream.str();
    #else
    user_id_temp_str = LangFile::getInstance()->getValue("user") + ": " + savefiles[save_selected].getUserName();
    #endif

    game_userid.setMsg(user_id_temp_str);
    game_userid.setPosition( (game_author.getSize().x / 2) - (game_userid.getSize().x / 2), game_author.getSize().y + SPACING );
}

void MainState::onNotify(const Signal& theSignal) {
    if( theSignal.getName() == "EXIT" ) requestToExit();

    if( theSignal.getName() == "RIGHT" && !savefiles.empty() && save_selected < savefiles.size() - 1 ) emitSignal(Signal("SCROLL_RIGHT"));
    if( theSignal.getName() == "SCROLL_RIGHT" ) { save_selected++; buildTitleInfo(); }

    if( theSignal.getName() == "LEFT" && !savefiles.empty() && save_selected > 0 ) emitSignal(Signal("SCROLL_LEFT"));
    if( theSignal.getName() == "SCROLL_LEFT" ) { save_selected--; buildTitleInfo(); }

    if( theSignal.getName() == "EXPORT" ) export_svi();
    //if( theSignal.getName() == "IMPORT" ) import_svi();
}

void MainState::export_svi() {
    char temp_buffer[256];

    std::string path = EXPORT_PATH + itoa(savefiles[save_selected].getTitleID(), temp_buffer, 16) + std::string(".svi");

    OPResult res = savefiles[save_selected].extractToSVIFile(path);
    std::string result_str;
    if( res ) result_str = LangFile::getInstance()->getValue("extract_success") + " " + path;
    else result_str = LangFile::getInstance()->getValue("extract_failed") + " " + std::string(itoa(res.getErrorNumber(), temp_buffer, 10)) + "-" + std::string(itoa(res.getLibNXErrorNumber(), temp_buffer, 10));

    ResultState* res_state = new ResultState(result_str);
    Gui::getInstance()->addState(res_state);
}

/*void MainState::import_svi() {
    ImportState* import_state = new ImportState(gui_ptr, savefiles[save_selected]);
    gui_ptr->addState(import_state);
}*/
