#ifndef _LAYER_HPP_
#define _LAYER_HPP_

#include <vector>

#include "drawable.hpp"
#include "node.hpp"

/*A layer of graphics, this is an helper class for scene, it does not need to be instantiated directly*/

class Scene;
class Layer : public Drawable {
    private:
        Layer() = default;

        friend class Scene;
        std::vector<Node*> nodes;

        virtual void draw(RenderSurface& theTarget, const Transformation& theTransformation = Transformation()) const final;
        virtual void update(const unsigned int theFrameElapsed);

};

#endif // _LAYER_HPP_
