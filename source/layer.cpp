#include "layer.hpp"

void Layer::draw(RenderSurface& theTarget, const Transformation& theTransformation) const {
    for( auto it = nodes.begin(); it < nodes.end(); it++ )
        theTarget.draw(*(*it), theTransformation);
}
