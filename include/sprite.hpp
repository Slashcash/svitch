#ifndef _SPRITE_HPP_
#define _SPRITE_HPP_

#include "node.hpp"
#include "texture.hpp"

/*the implementation of a sprite PLEASE NOTE THAT THIS JUST STORES A REFERENCE TO A TEXTURE*/

class Sprite : public Node {
    private:
        const Texture* texture;

        virtual void drawCurrent(RenderSurface& theTarget, const Transformation& theTransformation) const;

    public:
        Sprite() : Node() { texture = nullptr; }
        Sprite(const Texture& theTexture) : Node() { texture = &theTexture; }

        const Texture* getTexture() const { return texture; }

        void setTexture(const Texture& theTexture) { texture = &theTexture; }

        virtual Size getSize() const { if( texture == nullptr ) return Size(0, 0); else return texture->getSize(); }
};

#endif // _SPRITE_HPP_
