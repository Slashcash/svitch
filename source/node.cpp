#include "node.hpp"

Node::Node() : Transformation(), Drawable() {
    parent = nullptr;
    is_active = true;
    frames_elapsed = 0;
    frames_update = 0;
}

void Node::draw(RenderSurface& theTarget, const Transformation& theTransformation) const {
    //drawing this node
    Transformation thisnode_transformation = (*this) * theTransformation;
    drawCurrent(theTarget, thisnode_transformation);

    //drawing all its children
    for( auto it = children.begin(); it < children.end(); it++ ) (*it)->draw(theTarget, thisnode_transformation);
}

void Node::detachChild(Node* theChild) {
    for( auto it = children.begin(); it < children.end(); it++ )
        if( *it == theChild ) {
            (*it)->parent = nullptr;
            children.erase(it);
        }
}

Transformation Node::getTotalTransformation() const {
    Node* prev = getParent();
    Transformation result = *this;

    while( prev != nullptr ) {
        result = result * (*prev);
        prev = prev->getParent();
    }

    return result;
}

void Node::update(const unsigned int theFrameElapsed) {
    frames_elapsed = frames_elapsed + theFrameElapsed;

    if( (is_active) && (frames_elapsed >= frames_update || frames_update == 0) ) {
        updateCurrent();
        frames_elapsed = 0;
    }

    for( auto it = children.begin(); it < children.end(); it++ ) (*it)->update(theFrameElapsed);
}
