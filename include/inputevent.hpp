#ifndef _INPUTEVENT_HPP_
#define _INPUTEVENT_HPP_

#include <switch/services/hid.h>

#include "node.hpp" //for the Bound class

typedef HidControllerKeys Button; //all the nintendo switch analog buttons
static const Button BUTTON_A = KEY_A;
static const Button BUTTON_B = KEY_B;
static const Button BUTTON_X = KEY_X;
static const Button BUTTON_Y = KEY_Y;
static const Button BUTTON_LSTICK = KEY_LSTICK;
static const Button BUTTON_RSTICK = KEY_RSTICK;
static const Button BUTTON_L = KEY_L;
static const Button BUTTON_R = KEY_R;
static const Button BUTTON_ZL = KEY_ZL;
static const Button BUTTON_ZR = KEY_ZR;
static const Button BUTTON_PLUS = KEY_PLUS;
static const Button BUTTON_MINUS = KEY_MINUS;
static const Button BUTTON_DUP = KEY_DUP;
static const Button BUTTON_DDOWN = KEY_DDOWN;
static const Button BUTTON_DLEFT = KEY_DLEFT;
static const Button BUTTON_DRIGHT = KEY_DRIGHT;
static const Button BUTTON_LSTICK_UP = KEY_LSTICK_UP;
static const Button BUTTON_LSTICK_DOWN = KEY_LSTICK_DOWN;
static const Button BUTTON_LSTICK_LEFT = KEY_LSTICK_LEFT;
static const Button BUTTON_LSTICK_RIGHT = KEY_LSTICK_RIGHT;
static const Button BUTTON_RSTICK_UP = KEY_RSTICK_UP;
static const Button BUTTON_RSTICK_DOWN = KEY_RSTICK_DOWN;
static const Button BUTTON_RSTICK_LEFT = KEY_RSTICK_LEFT;
static const Button BUTTON_RSTICK_RIGHT = KEY_RSTICK_RIGHT;
static const Button BUTTON_SL = KEY_SL;
static const Button BUTTON_SR = KEY_SR;

enum Player {   //all the nintendo switch players
    PLAYER1 //just supporting player1 for now :(
};

class InputEvent {
    public:
        enum Type { //an input event can be of different types
            UNKNOWN,
            BUTTON_PRESSED,
            BUTTON_RELEASED,
            WINDOW_CLOSE,
            TOUCH_INPUT_PRESSED,
            TOUCH_INPUT_RELEASED
        };

    private:
        Type input_type;                    //the type of input
        Button input_button;                //unknown if this event does not refer to a button
        Player input_player;                //the player which this event refers to, meaningless for some event types
        Bound touch_bound;

    public:
        InputEvent(const Type& theEventType = UNKNOWN, const Button& theButton = BUTTON_A, const Player& thePlayer = Player::PLAYER1);

        Type getInputType() const { return input_type; } //ALWAYS CHECK THIS FIRST, IF THIS IS UNKNOWN OTHER FIELDS ARE MEANINGLESS
        Button getButton() const { return input_button; } //if the type regards a button, this will return the affected button
        Player getPlayer() const { return input_player; } //only meaningful for some kind of types
        Bound getTouchBound() const { return touch_bound; }

        void setType(const Type& theType) { input_type = theType; }
        void setButton(const Button& theButton) { input_button = theButton; }
        void setPlayer(const Player& thePlayer) { input_player = thePlayer; }
        void setTouchBound(const Bound& theTouchBound) { touch_bound = theTouchBound; }

        bool operator==(const InputEvent& theEvent) const;
};

#endif // _INPUTEVENT_HPP_
