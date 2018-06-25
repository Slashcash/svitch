#include "node.hpp"

bool Bound::intersects(const Bound& theBound) const {
    if( top_left.x + size.x < theBound.top_left.x || theBound.top_left.x + theBound.size.x < top_left.x || top_left.y + size.y < theBound.size.y || theBound.top_left.y + theBound.size.y < top_left.y )
        return false;

    else return true;
}

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

    applyTransitions(theFrameElapsed);

    if( (is_active) && (frames_elapsed >= frames_update || frames_update == 0) ) {
        updateCurrent();
        frames_elapsed = 0;
    }

    for( auto it = children.begin(); it < children.end(); it++ ) (*it)->update(theFrameElapsed);
}

void Node::applyTransitions(const long unsigned int theElapsedFrame) {
    //erasing all the completed transformations
    for( auto it = transitions.begin(); it < transitions.end(); it++ )
        if( it->transformation_queue.empty() )
            transitions.erase(it);

    for( auto it = transitions.begin(); it < transitions.end(); it++ ) {
        it->update_count = it->update_count + theElapsedFrame;

        if( it->update_count >= it->update_rate || it->update_rate == 0 ) {
            Transformation current_transformation = it->transformation_queue.front();

            move(current_transformation.getPosition());
            scale(current_transformation.getScaling());
            rotate(current_transformation.getRotation());

            it->transformation_queue.pop();
            it->update_count = 0;
        }
    }
}

void Node::addTransition(const long unsigned int theUpdateRate, std::queue<Transformation> theTransformationQueue) {
    Transition buffer;
    buffer.update_rate = theUpdateRate;
    buffer.update_count = 0;
    buffer.transformation_queue = theTransformationQueue;

    transitions.push_back(buffer);
}

std::vector<Node*> Node::beenTouched(const Bound& theTouchPosition) {
    std::vector<Node*> buffer;

    //checking if the node itself has been touched
    if( getGlobalBound().intersects(theTouchPosition) ) buffer.push_back(this);

    //checking for its children
    for( auto it = children.begin(); it < children.end(); it++ )
        if( (*it)->getGlobalBound().intersects(theTouchPosition) ) buffer.push_back(*it);

    return buffer;
}
