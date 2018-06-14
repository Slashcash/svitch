#include "mainstate.hpp"

#include <stdlib.h>
#include <dirent.h>
#include <algorithm>

#include "gui.hpp"
#include "savefile.hpp"
#include "options.cpp"
#include "langfile.hpp"
#include "resultstate.hpp"
#include "importstate.hpp"

MainState::MainState() {
    //variable initialization
    save_selected = 0;

    //setting inputs
    std::vector<InputSignal> signals;
    //why changing buttons? because yuzu is a bitch
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
    savefiles = SaveFile::getAllSaveFiles();

    //loading all the icons of the savefiles
    for( auto it = savefiles.begin(); it < savefiles.end(); it++ ) {
        std::ostringstream id;
        id << it->getTitleID();
        std::string temp_icon = it->getIcon();
        texture_manager.addResource((void*)(&(temp_icon[0])), temp_icon.size(), id.str());
    }

    #ifndef EMULATOR
    //loading all the icons of the users
    system_accounts = SaveFile::getAccounts();
    for( auto it = system_accounts.begin(); it < system_accounts.end(); it++ ) {
        std::string temp_icon = it->icon;
        texture_manager.addResource((void*)(&(temp_icon[0])), temp_icon.size(), it->name);
    }
    #endif

    buildScreen();
}

void MainState::onNotify(const Signal& theSignal) {
    if( theSignal.getName() == "RIGHT" && !savefiles.empty() && save_selected < savefiles.size() - 1 ) emitSignal(Signal("SCROLL_RIGHT"));
    if( theSignal.getName() == "SCROLL_RIGHT" ) changeSelected(save_selected+1);

    if( theSignal.getName() == "LEFT" && !savefiles.empty() && save_selected > 0 ) emitSignal(Signal("SCROLL_LEFT"));
    if( theSignal.getName() == "SCROLL_LEFT" ) changeSelected(save_selected-1);
}

void MainState::changeSelected(unsigned int theSelection) {
    /*int move_factor = save_selected - theSelection;
    save_selected = theSelection;

    //building the transition
    const unsigned int TRANSITION_NUM = 6;
    const unsigned int TRANSITION_LENGTH = 1; //in seconds;
    const unsigned int MOVEMENT = (cover_arts.front().getSize().x + FRAME_TO_FRAME_SPACING)*move_factor / TRANSITION_NUM;
    std::queue<Transformation> transformation_queue;

    for(unsigned int i = 0; i < TRANSITION_NUM; i++) transformation_queue.push(Transformation(Coordinate(MOVEMENT, 0)));
    cover_arts.front().addTransition(60*TRANSITION_LENGTH / TRANSITION_NUM, transformation_queue);

    const float SCALE_FACTOR = 1.2;

    for(auto it = cover_arts.begin(); it < cover_arts.end(); it++) it->setScale(1, 1);
    cover_arts[save_selected].setScale(SCALE_FACTOR, SCALE_FACTOR);
    cover_arts[save_selected+1].setScale(1/SCALE_FACTOR, 1/SCALE_FACTOR);

    buildTitleInfo();*/
}

std::string MainState::chooseExportFileName() const {
    char temp_buffer[256];

    const std::string DEFAULT_FILENAME = itoa(savefiles[save_selected].getTitleID(), temp_buffer, 16);
    std::string chosen_name = DEFAULT_FILENAME;
    std::vector<std::string> file_cover_names_in_folder;

    //getting all the filecover_names in the export folder
    DIR* d = opendir(EXPORT_PATH.c_str()); // open the path
    if( d == NULL ) return chosen_name + ".svi";

    dirent* dir; // for the directory entries

    while((dir = readdir(d)) != NULL) file_cover_names_in_folder.push_back(dir->d_name);

    //choosing an unique name
    bool found = false;
    int i = 1;
    while( !found ) {
        if( std::find(file_cover_names_in_folder.begin(), file_cover_names_in_folder.end(), chosen_name + ".svi") == file_cover_names_in_folder.end() ) found = true; //if there is nothing with the same name the name is unique :D
        else { //otherwise we try all the variations like this NAME_1, NAME_2...
            std::ostringstream temp_str;
            temp_str << DEFAULT_FILENAME << "_" << i;
            chosen_name = temp_str.str();
            i++;
        }
    }

    return chosen_name + ".svi";
}

void MainState::buildCarousel() {
    const unsigned int COVERART_WIDTH = 256;
    const unsigned int COVERART_HEIGHT = 256;
    const std::string FONT = FONT_PATH + "roboto.ttf";
    const unsigned int CHAR_SIZE = 15;
    const Color CHAR_COLOR = Color::WHITE;
    const unsigned int FRAME_TO_TEXT_SPACING = 25;
    const unsigned int TEXT_TO_TEXT_SPACING = 15;
    const int LATERAL_SPACING = 0;
    const unsigned int BORDER_SPACE = 98;
    const unsigned int ELEMENT_PER_PAGE = 4;
    const unsigned int FRAME_TO_FRAME_SPACING = 20;

//building the carousel
    cover_arts.clear();
    cover_arts.reserve(savefiles.size());

    cover_names.clear();
    cover_names.reserve(savefiles.size());

    cover_authors.clear();
    cover_authors.reserve(savefiles.size());

    unsigned int i = 0;
    for( auto it = savefiles.begin(); it < savefiles.end(); it++ ) {
        std::ostringstream temp;
        temp << it->getTitleID();
        Texture& temp_texture = texture_manager.getResource(temp.str());
        if( temp_texture.getSize().x == 0 ) temp_texture = texture_manager.getResource(TEXTURE_PATH + "default_coverart.png"); //if we didn't find the cover art

        cover_arts.push_back(Sprite(temp_texture));
        cover_arts.back().setOrigin(cover_arts.back().getSize().x/2, cover_arts.back().getSize().y/2);
        if( (i % ELEMENT_PER_PAGE) == 0 ) cover_arts.back().setPosition(BORDER_SPACE + cover_arts.back().getSize().x / 2, scene.getSize().y /2 );
        else cover_arts.back().setPosition((cover_arts.end()-2)->getPosition().x + (cover_arts.end()-2)->getSize().x + FRAME_TO_FRAME_SPACING, scene.getSize().y /2);

        float temp_scale_x = float(COVERART_WIDTH / cover_arts.back().getSize().x);
        float temp_scale_y = float(COVERART_HEIGHT / cover_arts.back().getSize().y);
        cover_arts.back().setScale(temp_scale_x, temp_scale_y);

        cover_authors.push_back(Text(font_manager.getResource(FONT), it->getBestSuitableAuthor(Gui::getInstance()->getSystemLanguage())));
        cover_authors.back().setFixedWidth(COVERART_WIDTH);
        cover_authors.back().setCharacterSize(CHAR_SIZE);
        cover_authors.back().setColor(CHAR_COLOR);
        cover_authors.back().setPosition(-(cover_arts.back().getSize().x / 2) + LATERAL_SPACING, -(cover_arts.back().getSize().x / 2) - FRAME_TO_TEXT_SPACING);
        cover_arts.back().attachChild(&cover_authors.back());

        cover_names.push_back(Text(font_manager.getResource(FONT), it->getBestSuitableName(Gui::getInstance()->getSystemLanguage())));
        cover_names.back().setFixedWidth(COVERART_WIDTH);
        cover_names.back().setCharacterSize(CHAR_SIZE);
        cover_names.back().setColor(CHAR_COLOR);
        cover_names.back().setPosition(0, -cover_names.back().getSize().y -TEXT_TO_TEXT_SPACING);
        cover_authors.back().attachChild(&cover_names.back());

        i++;
    }

    buildCarouselPage(0);
}

void MainState::buildCarouselPage(const unsigned int thePage) {
    const unsigned int ELEMENT_PER_PAGE = 4;

    //destroying the old page
    unsigned int start_pos = carousel_page * ELEMENT_PER_PAGE;
    unsigned int end_position = ((carousel_page + 1) * ELEMENT_PER_PAGE);
    if( end_position > cover_arts.size() ) end_position = cover_arts.size();

    for(unsigned int i = start_pos; i < end_position; i++) {
        scene.detachFromLayer(&cover_arts[i], 1);
    }

    carousel_page = thePage;

    //building the new one
    start_pos = carousel_page * ELEMENT_PER_PAGE;
    end_position = ((carousel_page + 1) * ELEMENT_PER_PAGE);
    if( end_position > cover_arts.size() ) end_position = cover_arts.size();

    for(unsigned int i = start_pos; i < end_position; i++) {
        scene.addToLayer(&cover_arts[i], 1);
    }
}

void MainState::buildUsers() {
    const unsigned int BORDER_X_SPACE = 98;
    const unsigned int ELEMENT_PER_PAGE = 4;
    const unsigned int FRAME_TO_FRAME_SPACING = 20;
    const unsigned int BORDER_Y_SPACE = 100;
    const unsigned int USERICON_WIDTH = 100;
    const unsigned int USERICON_HEIGHT = 100;
    const unsigned int FRAME_TO_TEXT_SPACING = 25;
    const unsigned int LATERAL_SPACING = 0;
    const unsigned int CHAR_SIZE = 10;
    const Color CHAR_COLOR = Color::WHITE;
    const std::string FONT = FONT_PATH + "roboto.ttf";

    user_arts.clear();
    user_names.clear();

    #ifndef EMULATOR
    user_arts.reserve(system_accounts.size());
    user_names.reserve(system_accounts.size());

    unsigned int i = 0;
    for(auto it = system_accounts.begin(); it < system_accounts.end(); it++) {
        Texture& temp_texture = texture_manager.getResource(it->name);
        if( temp_texture.getSize().x == 0 ) temp_texture = texture_manager.getResource(TEXTURE_PATH + "default_usericon.png"); //if we didn't find the user icon

        user_arts.push_back(Sprite(temp_texture));
        user_arts.back().setOrigin(user_arts.back().getSize().x/2, user_arts.back().getSize().y/2);
        if( (i % ELEMENT_PER_PAGE) == 0 ) user_arts.back().setPosition(BORDER_X_SPACE + user_arts.back().getSize().x / 2, scene.getSize().y - user_arts.back().getSize().y / 2 - BORDER_Y_SPACE );
        else user_arts.back().setPosition((user_arts.end()-2)->getPosition().x + (user_arts.end()-2)->getSize().x + FRAME_TO_FRAME_SPACING, scene.getSize().y /2);

        float temp_scale_x = float(USERICON_WIDTH / cover_arts.back().getSize().x);
        float temp_scale_y = float(USERICON_HEIGHT / cover_arts.back().getSize().y);
        user_arts.back().setScale(temp_scale_x, temp_scale_y);

        user_names.push_back(Text(font_manager.getResource(FONT), it->name));
        user_names.back().setFixedWidth(USERICON_WIDTH);
        user_names.back().setCharacterSize(CHAR_SIZE);
        user_names.back().setColor(CHAR_COLOR);
        user_names.back().setPosition(-(user_arts.back().getSize().x / 2) + LATERAL_SPACING, cover_arts.back().getSize().x / 2 + FRAME_TO_TEXT_SPACING);
        user_arts.back().attachChild(&user_names.back());

        i++;
    }
    #else
    user_arts.reserve(ELEMENT_PER_PAGE + 2);
    user_names.reserve(ELEMENT_PER_PAGE + 2);

    for(unsigned int i = 0; i < ELEMENT_PER_PAGE + 2; i++) {
        user_arts.push_back(Sprite(texture_manager.getResource(TEXTURE_PATH + "default_usericon.png")));
        user_arts.back().setOrigin(user_arts.back().getSize().x/2, user_arts.back().getSize().y/2);
        if( (i % ELEMENT_PER_PAGE) == 0 ) user_arts.back().setPosition(BORDER_X_SPACE + user_arts.back().getSize().x / 2, scene.getSize().y - user_arts.back().getSize().y / 2 - BORDER_Y_SPACE );
        else user_arts.back().setPosition((user_arts.end()-2)->getPosition().x + (user_arts.end()-2)->getSize().x + FRAME_TO_FRAME_SPACING, scene.getSize().y /2);

        float temp_scale_x = float(USERICON_WIDTH / cover_arts.back().getSize().x);
        float temp_scale_y = float(USERICON_HEIGHT / cover_arts.back().getSize().y);
        user_arts.back().setScale(temp_scale_x, temp_scale_y);

        user_names.push_back(Text(font_manager.getResource(FONT), "TEST USER"));
        user_names.back().setFixedWidth(USERICON_WIDTH);
        user_names.back().setCharacterSize(CHAR_SIZE);
        user_names.back().setColor(CHAR_COLOR);
        user_names.back().setPosition(-(user_arts.back().getSize().x / 2) + LATERAL_SPACING, cover_arts.back().getSize().x / 2 + FRAME_TO_TEXT_SPACING);
        user_arts.back().attachChild(&user_names.back());
    }
    #endif // EMULATOR

    buildUsersPage(0);
}

void MainState::buildUsersPage(const unsigned int thePage) {
    const unsigned int ELEMENT_PER_PAGE = 4;

    //destroying the old page
    unsigned int start_pos = users_page * ELEMENT_PER_PAGE;
    unsigned int end_position = ((users_page + 1) * ELEMENT_PER_PAGE);
    if( end_position > user_arts.size() ) end_position = user_arts.size();

    for(unsigned int i = start_pos; i < end_position; i++) {
        scene.detachFromLayer(&user_arts[i], 1);
    }

    users_page = thePage;

    //building the new one
    start_pos = users_page * ELEMENT_PER_PAGE;
    end_position = ((users_page + 1) * ELEMENT_PER_PAGE);
    if( end_position > user_arts.size() ) end_position = user_arts.size();

    for(unsigned int i = start_pos; i < end_position; i++) {
        scene.addToLayer(&user_arts[i], 1);
    }
}

void MainState::buildSelector() {
    selector_sprite.setTexture(texture_manager.getResource(TEXTURE_PATH+"selector.png"));
    setSelector(0);
    scene.addToLayer(&selector_sprite, 0);
}

void MainState::setSelector(const unsigned int theSelection) {
    const int X_OFFSET = 0;
    const int Y_OFFSET = 0;

    selector_sprite.setPosition(cover_arts[theSelection].getPosition().x + X_OFFSET, cover_arts[theSelection].getPosition().y + Y_OFFSET);
}

void MainState::buildScreen() {
    if( !savefiles.empty() ) {
        buildCarousel();
        buildUsers();
        buildSelector();
    }
}
