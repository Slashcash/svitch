#ifndef _GUI_HPP_
#define _GUI_HPP_

/*The main gui class for this library*/

#include <stack>

class State;

class Gui {
    private:
        static Gui* instance;
        static bool is_initialized;
        std::stack<State*> states;

        Gui();
        ~Gui();

        void dropState();

    public:
        Gui(const Gui& theGui) = delete;
        Gui& operator=(const Gui& theGui) = delete;

        void addState(State* theState) { states.push(theState); } //theState should be on the heap

        void run();
        static Gui* getInstance();
};

#endif // _GUI_HPP_
