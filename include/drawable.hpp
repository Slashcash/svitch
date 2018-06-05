#ifndef _DRAWABLE_HPP_
#define _DRAWABLE_HPP_

#include "transformation.hpp"
#include "rendersurface.hpp"

class Drawable {
    private:
        friend class RenderSurface;
        virtual void draw(RenderSurface& theRenderSurface, const Transformation& theTransformation = Transformation()) const = 0;
};



#endif // _DRAWABLE_HPP_
