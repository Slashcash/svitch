#ifndef _TEXT_HPP_
#define _TEXT_HPP_

#include <string>

#include "node.hpp"
#include "font.hpp"
#include "color.hpp"

class Text : public Node {
    private:
        Font* font;
        std::string msg;
        unsigned int font_size;
        Color font_color;

        virtual void drawCurrent(RenderSurface& theTarget, const Transformation& theTransformation) const;

    public:
        Text() : Node() { font = nullptr; font_size = Font::DEFAULT_SCALABLE_SIZE; font_color = Color::WHITE; }
        Text(Font& theFont, const std::string& theMsg = "") : Node() { font = &theFont; msg = theMsg; font_size = Font::DEFAULT_SCALABLE_SIZE; font_color = Color::WHITE; }

        virtual Size getSize() const;
        unsigned int getCharacterSize() const { return font_size; }
        const Font* getFont() const { return font; }
        std::string getMsg() const { return msg; }
        Color getColor() const { return font_color; }

        void setFont(Font& theFont) { font = &theFont; }
        void setMsg(const std::string& theMsg) { msg = theMsg; }
        void setCharacterSize(const unsigned int theSize) { font_size = theSize; }
        void setColor(const Color& theColor) { font_color = theColor; }
};

#endif // _TEXT_HPP_
