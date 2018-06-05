#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <vector>

#include "node.hpp"
#include "layer.hpp"

/*a scene is just a vector of layers that can be drawn on a window*/

class Scene : public Node {
    private:
        std::vector<Layer> layers; //all the  layers from this scene
        virtual void drawCurrent(Window& theWindow, const Transformation& theTransformation = Transformation()) const final;
        Size scene_size; //the size of the scene, the scene will be scaled to fit it

    public:
        Scene(const Size& theSize = (Window::getInstance()->getSize())) : Node() { scene_size = theSize; }

        void addToLayer(Node* theNode, const unsigned int theLayerNum); //adds a node to the specified layer
        void detachFromLayer(const Node* theNode, const unsigned int theLayerNum); //detaches a node from the specified layer
        virtual Size getSize() const { return scene_size; }
        void setSize(const Size& theSize) { scene_size = theSize; }
};

#endif // _SCENE_HPP_
