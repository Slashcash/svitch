#ifndef _SIGNAL_HPP_
#define _SIGNAL_HPP_

#include <string>

/*An element of the UI can emit a signal that can be received by other UI elements that may want to react upon*/

class Signal {
    private:
        std::string signal_name;        //the signal name (which should represnt the signal univocally)

    public:
        Signal() = delete;
        Signal(const std::string& theSignalName) { signal_name = theSignalName; }

        void setName(const std::string& theName) { signal_name = theName; }

        std::string getName() const { return signal_name; }

        operator std::string() const { return signal_name; }
        bool operator==(const std::string& theString) { return getName() == theString; } //you can compare a string with a signal
};

#endif // _SIGNAL_HPP_
