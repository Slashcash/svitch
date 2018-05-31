#ifndef _DRAWABLE_HPP_
#define _DRAWABLE_HPP_

#include "transformation.hpp"
#include "window.hpp"

class Drawable {
    private:
        friend class Window;
        virtual void draw(Window& theWindow, const Transformation& theTransformation = Transformation()) const = 0;
};



#endif // _DRAWABLE_HPP_
