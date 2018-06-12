#include "layer.hpp"

void Layer::draw(RenderSurface& theTarget, const Transformation& theTransformation) const {
    for( auto it = nodes.begin(); it < nodes.end(); it++ )
        theTarget.draw(*(*it), theTransformation);
}

void Layer::update(const unsigned int theFrameElapsed) {
    for( auto it = nodes.begin(); it < nodes.end(); it++ )
        (*it)->update(theFrameElapsed);
}
