#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <vector>

#include "drawable.hpp"
#include "transformation.hpp"
#include "window.hpp"

/*the basic graphic element for this library, it inherits from transformation
nodes are organized in a tree, drawing a node will draw all its children, children transformation is relative to the parent's one*/

//the bounding box of a node
class Bound {
    public:
        Coordinate top_left;
        Size size;

        Bound() { top_left = Coordinate(0, 0); size = Size(0, 0); }
        Bound(const Coordinate& theCoordinate, const Size& theSize) { top_left = theCoordinate; size = theSize; }
};

class Node : public Transformation, public Drawable {
    private:
        Node* parent; //the parent node, lifecycle to be managed separately
        std::vector<Node*> children; //all the children. Lifecycle to be managed externally

        virtual void draw(Window& theWindow, const Transformation& theTransformation = Transformation()) const final;
        virtual void drawCurrent(Window& theWindow, const Transformation& theTransformation) const = 0; //derive and implement this
        virtual void setParent(Node* theParent) final { parent = theParent; }
        virtual Node* getParent() const final { return parent; }

    public:
        Node();

        virtual Size getSize() const = 0;
        virtual Bound getBound() const final { return Bound(getPosition(), getSize()); }
        virtual Bound getGlobalBound() const final { return Bound(getGlobalPosition(), getSize()); }
        virtual void attachChild(Node* theChild) final { children.push_back(theChild); theChild->setParent(this); }
        virtual void detachChild(Node* theChild) final;
        virtual void detachAllChildren() final { for(auto it = children.begin(); it < children.end(); it++) (*it)->parent = nullptr;  children.clear(); }
        virtual Transformation getTotalTransformation() const final;
        virtual Coordinate getGlobalPosition() const final { return getTotalTransformation().getPosition(); }
};

#endif // _NODE_HPP_
