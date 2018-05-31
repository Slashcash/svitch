#include "signaler.hpp"

#include "observer.hpp"

void Signaler::emitSignal(const Signal& theSignal) {
    for( auto it = observers.begin(); it < observers.end(); it++ )
        (*it)->onNotify(theSignal);
}
