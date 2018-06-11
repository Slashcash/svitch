#include "transformation.hpp"

#include <math.h>

Coordinate Transformation::transformCoordinate(const Coordinate& toBeTransformed) const {
    //the coordinate after the calculated transformation
    Coordinate calculated;

    //taking the origin into account
    calculated.x = toBeTransformed.x - getOrigin().x;
    calculated.y = toBeTransformed.y - getOrigin().y;

    //calculating the scaling
    calculated.x = calculated.x * getScaling().x;
    calculated.y = calculated.y * getScaling().y;

    //calculating the translation
    calculated.x = calculated.x + getPosition().x;
    calculated.y = calculated.y + getPosition().y;

    //calculating the rotation
    /*calculated.x = calculated.x * cos(getRotation()) - calculated.y * sin(getRotation());
    calculated.y = calculated.x * sin(getRotation()) + calculated.y * cos(getRotation());*/

    return calculated;
}
