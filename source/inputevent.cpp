#include "inputevent.hpp"

InputEvent::InputEvent(const Type& theEventType, const Button& theButton, const Player& thePlayer) {
    input_type = theEventType;
    input_button = theButton;
    input_player = thePlayer;
}

bool InputEvent::operator==(const InputEvent& theEvent) const {
    if( (input_type != TOUCH_INPUT_PRESSED) && (input_type != TOUCH_INPUT_RELEASED) ) return ((input_type == theEvent.input_type) && (input_button == theEvent.input_button) && (input_player == theEvent.input_player));
    else return touch_bound == theEvent.touch_bound;
}
