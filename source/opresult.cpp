#include "opresult.hpp"

OPResult::OPResult(const unsigned int theErrNumber, const unsigned int theLibNXErrNumber) {
    err_number = theErrNumber;
    err_number_libnx = theLibNXErrNumber;
}

std::string OPResult::str() const {
    std::ostringstream stream;
    stream << err_number << "-" << err_number_libnx;

    return stream.str();
}
