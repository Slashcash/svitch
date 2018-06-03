#include "state.hpp"

FontManager State::font_manager;

void State::updateBase() {
    InputEvent buffer;
    while( Window::getInstance()->getInputEvents(buffer) ) {
        for(auto it = input_signals.begin(); it < input_signals.end(); it++) {
            if( it->first == buffer ) {
                emitSignal(it->second);
            }
        }
    }

    update();
}
