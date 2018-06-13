#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <vector>
#include <queue>

#include "drawable.hpp"
#include "transformation.hpp"
#include "rendersurface.hpp"

/*the basic graphic element for this library, it inherits from transformation
nodes are organized in a tree, drawing a node will draw all its children, children transformation is relative to the parent's one*/

//the bounding box of a node
class Bound {
    public:
        Coordinate top_left;
        Size size;
        bool is_active; //whether or not the node gets automatically updated

        Bound() { top_left = Coordinate(0, 0); size = Size(0, 0); }
        Bound(const Coordinate& theCoordinate, const Size& theSize) { top_left = theCoordinate; size = theSize; }
};

class Node;
struct Transition {
    std::queue<Transformation> transformation_queue;
    long unsigned int update_rate;
    long unsigned int update_count;
};

class Node : public Transformation, public Drawable {
    private:
        Node* parent; //the parent node, lifecycle to be managed separately
        std::vector<Node*> children; //all the children. Lifecycle to be managed externally
        long unsigned int frames_elapsed;
        long unsigned int frames_update; //this node gets automatically updated everytime frames_elapesd == frames update;
        bool is_active; //whether or not the node gets automatically updated
        std::vector<Transition> transitions;

        virtual void draw(RenderSurface& theTarget, const Transformation& theTransformation = Transformation()) const final;
        virtual void updateCurrent() {} //override this if you need to update a node
        virtual void drawCurrent(RenderSurface& theTarget, const Transformation& theTransformation) const = 0; //derive and implement this
        virtual void setParent(Node* theParent) final { parent = theParent; }
        virtual Node* getParent() const final { return parent; }
        virtual void applyTransitions(const long unsigned int theElapsedFrame);

    public:
        Node();

        virtual void update(const unsigned int theFrameElapsed); //this should unfortunately be FINAL but since i am a lazy fuck it is not (the reason is in scene.hpp)
        virtual Size getSize() const = 0;
        virtual long unsigned int getUpdateRate() const { return frames_update; }
        virtual void setUpdateRate(const long unsigned int theUpdateRate) { frames_update = theUpdateRate; }
        virtual Bound getBound() const final { return Bound(getPosition(), getSize()); }
        virtual Bound getGlobalBound() const final { return Bound(getGlobalPosition(), getSize()); }
        virtual void attachChild(Node* theChild) final { children.push_back(theChild); theChild->setParent(this); }
        virtual void detachChild(Node* theChild) final;
        virtual void detachAllChildren() final { for(auto it = children.begin(); it < children.end(); it++) (*it)->parent = nullptr;  children.clear(); }
        virtual Transformation getTotalTransformation() const final;
        virtual Coordinate getGlobalPosition() const final { return getTotalTransformation().getPosition(); }
        virtual void addTransition(const long unsigned int theUpdateRate, std::queue<Transformation> theTransformationQueue);
};

#endif // _NODE_HPP_
