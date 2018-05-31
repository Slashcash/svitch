#ifndef _SIGNALER_HPP_
#define _SIGNALER_HPP_

#include <vector>

#include "signal.hpp"

/*An element of the UI that may want to sends signals has to inherit from this*/

class Observer;

class Signaler {
    private:
        std::vector<Observer*> observers;       //here we will store all the subscribed observers (lifecycle managed externally)

    public:
        virtual void subscrive(Observer* theObserver) final { observers.push_back(theObserver); }
        virtual void emitSignal(const Signal& theSignal) final;

};

#endif // _SIGNALER_HPP_


