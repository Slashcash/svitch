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

    buildCarousel();
    changeSelected(0);
}

void MainState::onNotify(const Signal& theSignal) {
    if( theSignal.getName() == "RIGHT" && !savefiles.empty() && save_selected < savefiles.size() - 1 ) emitSignal(Signal("SCROLL_RIGHT"));
    if( theSignal.getName() == "SCROLL_RIGHT" ) changeSelected(save_selected+1);

    if( theSignal.getName() == "LEFT" && !savefiles.empty() && save_selected > 0 ) emitSignal(Signal("SCROLL_LEFT"));
    if( theSignal.getName() == "SCROLL_LEFT" ) changeSelected(save_selected-1);
}

void MainState::changeSelected(unsigned int theSelection) {
    const float SCALE_FACTOR = 1.2;

    int move_factor = save_selected - theSelection;
    save_selected = theSelection;

    cover_arts_frames.front().move((cover_arts_frames.front().getSize().x + FRAME_TO_FRAME_SPACING)*move_factor, 0);

    for(auto it = cover_arts_frames.begin(); it < cover_arts_frames.end(); it++) it->setScale(1, 1);
    cover_arts_frames[save_selected].setScale(SCALE_FACTOR, SCALE_FACTOR);
    cover_arts_frames[save_selected+1].setScale(1/SCALE_FACTOR, 1/SCALE_FACTOR);
}

std::string MainState::chooseExportFileName() const {
    char temp_buffer[256];

    const std::string DEFAULT_FILENAME = itoa(savefiles[save_selected].getTitleID(), temp_buffer, 16);
    std::string chosen_name = DEFAULT_FILENAME;
    std::vector<std::string> file_names_in_folder;

    //getting all the filenames in the export folder
    DIR* d = opendir(EXPORT_PATH.c_str()); // open the path
    if( d == NULL ) return chosen_name + ".svi";

    dirent* dir; // for the directory entries

    while((dir = readdir(d)) != NULL) file_names_in_folder.push_back(dir->d_name);

    //choosing an unique name
    bool found = false;
    int i = 1;
    while( !found ) {
        if( std::find(file_names_in_folder.begin(), file_names_in_folder.end(), chosen_name + ".svi") == file_names_in_folder.end() ) found = true; //if there is nothing with the same name the name is unique :D
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
    //building the carousel
    if( !cover_arts_frames.empty() ) scene.detachChild(&cover_arts_frames.front()); //just deleting the first to delete all the carousel
    cover_arts_frames.clear();
    cover_arts.clear();
    cover_arts_frames.reserve(savefiles.size());
    cover_arts.reserve(savefiles.size());
    const int FRAME_COVERART_SPACING = 10;
    for( auto it = savefiles.begin(); it < savefiles.end(); it++ ) {
        std::ostringstream temp;
        temp << it->getTitleID();

        cover_arts_frames.push_back(Sprite(texture_manager.getResource(TEXTURE_PATH+"coverart_frame.png")));
        cover_arts_frames.back().setOrigin(cover_arts_frames.back().getSize().x/2, cover_arts_frames.back().getSize().y/2);

        cover_arts.push_back(Sprite(texture_manager.getResource(temp.str())));
        cover_arts.back().setOrigin(cover_arts.back().getSize().x/2, cover_arts.back().getSize().y/2);

        if( it == savefiles.begin() ) {
            cover_arts_frames.back().setPosition(scene.getSize().x / 2, scene.getSize().y / 2);
            scene.addToLayer(&cover_arts_frames.back(), 0);
        }

        else {
            cover_arts_frames.back().setPosition((cover_arts_frames.end()-2)->getSize().x + FRAME_TO_FRAME_SPACING, 0);
            (cover_arts_frames.end()-2)->attachChild(&cover_arts_frames.back());
        }

        cover_arts.back().setPosition(0, 0);
        float temp_scale_x = float((cover_arts_frames.back().getSize().x - FRAME_COVERART_SPACING*2 )) / cover_arts.back().getSize().x;
        float temp_scale_y = float((cover_arts_frames.back().getSize().y - FRAME_COVERART_SPACING*2 )) / cover_arts.back().getSize().y;
        cover_arts.back().setScale(temp_scale_x, temp_scale_y);
        cover_arts_frames.back().attachChild(&cover_arts.back());
    }
}
