#include "state.hpp"

FontManager State::font_manager;
TextureManager State::texture_manager;

void State::updateBase(const long unsigned int theElapsedFrame) {
    InputEvent buffer;
    while( Window::getInstance()->getInputEvents(buffer) ) {
        for(auto it = input_signals.begin(); it < input_signals.end(); it++) {
            if( it->first == buffer ) {
                emitSignal(it->second);
            }
        }
    }

    scene.update(theElapsedFrame);

    update();
}
