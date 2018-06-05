#include "transformation.hpp"

#include <math.h>

Coordinate Transformation::transformCoordinate(const Coordinate& toBeTransformed) const {
    //the coordinate after the calculated transformation
    Coordinate calculated;

    //calculating the translation
    calculated.x = toBeTransformed.x + getPosition().x;
    calculated.y = toBeTransformed.y + getPosition().y;

    //calculating the scaling
    calculated.x = calculated.x * getScaling().x;
    calculated.y = calculated.y * getScaling().y;

    //calculating the rotation
    /*calculated.x = calculated.x * cos(getRotation()) - calculated.y * sin(getRotation());
    calculated.y = calculated.x * sin(getRotation()) + calculated.y * cos(getRotation());*/

    return calculated;
}
