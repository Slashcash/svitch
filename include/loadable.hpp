#ifndef _LOADABLE_HPP_
#define _LOADABLE_HPP_

#include <string>

#include "opresult.hpp"

/*every entity of this library that can be loaded externally from a file should inherit from this*/

class Loadable {
    public:
        virtual OPResult loadFromMemory(void* theData, const std::size_t theSize) = 0;
        virtual OPResult loadFromFile(const std::string& thePath) = 0;
};



#endif // _LOADABLE_HPP_
