#ifndef _STATE_HPP_
#define _STATE_HPP_

#include <vector>

#include "scene.hpp"
#include "observer.hpp"
#include "signaler.hpp"
#include "loadablemanager.hpp"
#include "window.hpp"

/*This class represents the different states an application can be in*/

typedef std::pair<InputEvent, Signal> InputSignal; //an InputSignal is a pair of an inputevent and a signal, the state will generate a theSignal when theInputEvent occures

class State : public Signaler, public Observer {
    private:
        friend class Gui;
        bool request_to_exit; //if set to true this state will be popped out as soon as possible
        bool request_to_shutdown; //if set to true the program will shut down as soon as possible

    protected:
        Scene scene;
        std::vector<InputSignal> input_signals;
        static FontManager font_manager;
        static TextureManager texture_manager;

    public:
        State() { subscrive(this); request_to_exit = false; request_to_shutdown = false; }

        virtual void drawBase() const final { Window::getInstance()->draw(scene); draw(); }
        virtual void draw() const {} //override this if you need some additional drawing
        virtual void updateBase() final;
        virtual void update() {} //override this if you need additional upgrading
        virtual void onNotify(const Signal& theSignal) {} //implement this if you need signal handling
        void setInputSignal(const std::vector<InputSignal>& theInputSignal) { input_signals.clear(); for( auto it = theInputSignal.begin(); it < theInputSignal.end(); it++ ) input_signals.push_back(*it); }

        bool isRequestedToExit() const { return request_to_exit; }
        bool isRequestedToShutdown() const { return request_to_shutdown; }

        void requestToExit() { request_to_exit = true; }
        void requestToShutDown() { request_to_shutdown = true; }

        virtual ~State() {}
};

#endif // _STATE_HPP_
