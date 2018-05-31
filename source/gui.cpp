#include "gui.hpp"

#include "state.hpp"
#include "langfile.hpp"
#include "options.cpp"
#include "mainstate.hpp"

Gui* Gui::instance = nullptr;
bool Gui::is_initialized = false;

Gui::Gui() {
    if( !Window::getInstance()->isValid() ) return;
    if( !LangFile::getInstance()->loadFromFile(ROMFS_PATH+"lang/eng.lang") ) return;

    addState(new MainState());
}

void Gui::run() {
    while( Window::getInstance()->isOpen() && !states.empty() ) {
        //updating
        states.top()->updateBase();

        //drawing
        Window::getInstance()->clear();
        states.top()->drawBase();
        Window::getInstance()->update();

        //checking if the current state requested an exit
        if( states.top()->isRequestedToExit() ) dropState();
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
