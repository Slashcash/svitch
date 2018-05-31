#include "resultstate.hpp"

#include "options.cpp"
#include "langfile.hpp"

ResultState::ResultState(const std::string& theString) {
    //setting the inputs
    std::vector<InputSignal> signals;
    InputEvent event(InputEvent::BUTTON_PRESSED, KEY_A);
    Signal signal("EXIT");
    signals.push_back(std::make_pair(event, signal));

    setInputSignal(signals);

    //writing the string
    text.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    text.setMsg(theString);
    scene.addToLayer(&text, 0);
    text.setPosition( (Window::getInstance()->getSize().x / 2) - (text.getSize().x /2), (Window::getInstance()->getSize().y / 2) - (text.getSize().y / 2) );

    press_a.setFont(font_manager.getResource(ROMFS_PATH+"fonts/roboto.ttf"));
    press_a.setMsg(LangFile::getInstance()->getValue("press_a_continue"));
    scene.addToLayer(&press_a, 0);
    press_a.setPosition( (text.getGlobalPosition().x) + (text.getSize().x / 2) - (press_a.getSize().x /2), 600 );
}

void ResultState::onNotify(const Signal& theSignal) {
    if( theSignal.getName() == "EXIT" ) requestToExit();
}

