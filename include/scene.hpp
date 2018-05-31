#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <vector>

#include "drawable.hpp"
#include "layer.hpp"

/*a scene is just a vector of layers that can be drawn on a window*/

class Scene : public Drawable {
    private:
        std::vector<Layer> layers; //all the  layers from this scene
        virtual void draw(Window& theWindow, const Transformation& theTransformation = Transformation()) const final;

    public:
        Scene() : Drawable() {}

        void addToLayer(Node* theNode, const unsigned int theLayerNum); //adds a node to the specified layer
        void detachFromLayer(const Node* theNode, const unsigned int theLayerNum); //detaches a node from the specified layer
};

#endif // _SCENE_HPP_
