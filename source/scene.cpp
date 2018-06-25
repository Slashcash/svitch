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
}

std::vector<Node*> Scene::getTouched(const InputEvent& theEvent) const {
    std::vector<Node*> buffer;

    if( theEvent.getInputType() == InputEvent::TOUCH_INPUT_PRESSED ) {
        for( auto it = layers.begin(); it < layers.end(); it++ ) {
            for( auto it2 = it->nodes.begin(); it2 < it->nodes.end(); it2++ ) {
                std::vector<Node*> temp = (*it2)->beenTouched(theEvent.getTouchBound());
                buffer.insert(buffer.end(), temp.begin(), temp.end());
            }
        }
    }


    return buffer;
}
