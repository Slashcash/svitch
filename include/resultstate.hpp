#ifndef _RESULTSTATE_HPP_
#define _RESULTSTATE_HPP_

#include <string>

#include "state.hpp"
#include "text.hpp"

class ResultState : public State {
    private:
        Text text;
        Text press_a;

    public:
        ResultState(const std::string& theString);

        virtual void onNotify(const Signal& theSignal);
};

#endif // _ERRORSTATE_HPP_

