#ifndef _RENDERSURFACE_HPP_
#define _RENDERSURFACE_HPP_

#include "transformation.hpp"
#include "color.hpp"

class Drawable;

class RenderSurface {
    protected:
        Size render_size;

    private:
        virtual int mapCoordinatesToLinear(const Coordinate& theCoordinate) const final; //converts a coordinate into a position in the linear framebuffer
        virtual void setPixelCurrent(const int thePosition, const Color& theColor) = 0;
        virtual Color getPixelCurrent(const int thePosition) = 0;

    public:
        virtual void clear() = 0;
        virtual void draw(const Drawable& theDrawable, const Transformation& theTransformation = Transformation()) final;
        virtual void setPixel(const Coordinate& theCoordinate, const Color& theColor, const Transformation& theTransformation = Transformation()) final; //fills the specified pixel window with the specified color/alpha (takes theTransformation into account)
        virtual Color getPixel(const Coordinate& theCoordinate, const Transformation& theTransformation = Transformation()) final;
        Size getSize() const { return render_size; }
};

#endif
