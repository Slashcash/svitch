#include "node.hpp"

Node::Node() : Transformation(), Drawable() {
    parent = nullptr;
}

void Node::draw(Window& theWindow, const Transformation& theTransformation) const {
    //drawing this node
    Transformation thisnode_transformation = (*this) * theTransformation;
    drawCurrent(theWindow, thisnode_transformation);

    //drawing all its children
    for( auto it = children.begin(); it < children.end(); it++ ) (*it)->draw(theWindow, thisnode_transformation);
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
