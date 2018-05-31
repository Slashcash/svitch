#ifndef _OBSERVER_HPP_
#define _OBSERBER_HPP_

#include "signal.hpp"

/*an observer subscribes to a signaler to receive its signals, it may react upon them in the onNotify function that should be implemented*/

class Observer {
    public:
        virtual void onNotify(const Signal& theSignal) = 0; //implement this
};

#endif // _OBSERVER_HPP_
