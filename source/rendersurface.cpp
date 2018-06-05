#include "rendersurface.hpp"

#include "drawable.hpp"

int RenderSurface::mapCoordinatesToLinear(const Coordinate& theCoordinate) const {
    if( theCoordinate.x < 0 || theCoordinate.y < 0 ) return -1; //if it is outside the range of our window in negative
    if( (unsigned int)theCoordinate.x >= render_size.x || (unsigned int)theCoordinate.y >= render_size.y ) return -1; //or in positive (safe cast now because we know coordinate are both positive)

    else return theCoordinate.y * render_size.x + theCoordinate.x;
}

void RenderSurface::draw(const Drawable& theDrawable, const Transformation& theTransformation) {
        theDrawable.draw(*this, theTransformation);
}

void RenderSurface::setPixel(const Coordinate& theCoordinate, const Color& theColor, const Transformation& theTransformation) {
    int pos = mapCoordinatesToLinear(theTransformation.transformCoordinate(theCoordinate));

    if( pos != -1 ) setPixelCurrent(pos, theColor);
}

