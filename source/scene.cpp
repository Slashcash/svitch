#include "scene.hpp"

void Scene::detachFromLayer(const Node* theNode, const unsigned int theLayerNum) {
    if( theLayerNum >= 0 && theLayerNum < layers.size() ) //making sure it is in range
        for( auto it = layers[theLayerNum].nodes.begin(); it < layers[theLayerNum].nodes.end(); it++ ) //scrolling the layer to find our node
            if( theNode == *it )
                layers[theLayerNum].nodes.erase(it); //erasing it
}

void Scene::addToLayer(Node* theNode, const unsigned int theLayerNum) {
    if( theLayerNum >= 0 ) //making sure (pleonastic?) that we are in the vector range
        if( theLayerNum >= layers.size() ) while( layers.size() <= theLayerNum ) layers.push_back(Layer()); //if this layer does not exists already we create it

    layers[theLayerNum].nodes.push_back(theNode);

}

void Scene::drawCurrent(RenderSurface& theTarget, const Transformation& theTransformation) const {
    //does it need to be scaled taking its size into account?
    /*Vector2<float> scaling_factor = Vector2<float>(getSize().x / theTarget.getSize().x, getSize().y / theTarget.getSize().y);
    Transformation calculated_transf = theTransformation;
    calculated_transf.scale(scaling_factor);*/

    //DECOMMENTING THIS SHIT RIGHT UP HERE WILL ENABLE THE AUTOSCALING TO A FIXED RESOLUTION, SINCE WE DON'T HAVE A GOOD DOWN/UPSCALING ALGORITHM RIGHT NOW IT IS DISABLED

    for( auto it = layers.begin(); it < layers.end(); it++ )
        theTarget.draw((*it), theTransformation);
}

void Scene::update(const long unsigned theFrameElapsed) {
    for( auto it = layers.begin(); it < layers.end(); it++ )
        it->update(theFrameElapsed);

    applyTransitions(theFrameElapsed);
}

void Scene::applyTransitions(const long unsigned int theElapsedFrame) {
    //erasing all the completed transformations
    for( auto it = transitions.begin(); it < transitions.end(); it++ )
        if( it->transformation_queue.empty() )
            transitions.erase(it);

    for( auto it = transitions.begin(); it < transitions.end(); it++ ) {
        it->update_count = it->update_count + theElapsedFrame;

        if( it->update_count >= it->update_rate || it->update_rate == 0 ) {
            Transformation current_transformation = it->transformation_queue.front();

            it->element->move(current_transformation.getPosition());
            it->element->scale(current_transformation.getScaling());
            it->element->rotate(current_transformation.getRotation());

            it->transformation_queue.pop();
            it->update_count = 0;
        }
    }
}

void Scene::addTransition(Node* theNode, const long unsigned int theUpdateRate, std::queue<Transformation> theTransformationQueue) {
    Transition buffer;
    buffer.update_rate = theUpdateRate;
    buffer.element = theNode;
    buffer.update_count = 0;
    buffer.transformation_queue = theTransformationQueue;

    transitions.push_back(buffer);
}
