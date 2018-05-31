#include "layer.hpp"

void Layer::draw(Window& theWindow, const Transformation& theTransformation) const {
    for( auto it = nodes.begin(); it < nodes.end(); it++ )
        theWindow.draw(*(*it), theTransformation);
}
