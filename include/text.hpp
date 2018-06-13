#ifndef _TEXT_HPP_
#define _TEXT_HPP_

#include <string>

#include "node.hpp"
#include "font.hpp"
#include "color.hpp"

class Text : public Node {
    private:
        Font* font;
        std::vector<uint32_t> msg;
        unsigned int font_size;
        Color font_color;
        unsigned int fixed_width; //setting this to a value != 0 will truncate this text
        std::size_t end_position;

        virtual void drawCurrent(RenderSurface& theTarget, const Transformation& theTransformation) const;
        std::vector<uint32_t> convertToUTF8(const std::string& theMSG);
        Size getSizeByIndex(const unsigned int theStartPosition, const unsigned int theEndPosition) const; //used to calculate the fixed width
        void calculateFixedWidth();

    public:
        Text();
        Text(Font& theFont, const std::string& theMsg = "");

        virtual Size getSize() const;
        unsigned int getCharacterSize() const { return font_size; }
        const Font* getFont() const { return font; }
        //std::string getMsg() const { return msg; } //because i'm so lazy to encode an utf8 back to a normal string
        unsigned int getFixedWidth() const { return fixed_width; }
        Color getColor() const { return font_color; }

        void setFont(Font& theFont) { font = &theFont; calculateFixedWidth(); }
        void setMsg(const std::string& theMsg) { msg = convertToUTF8(theMsg); end_position = msg.size(); calculateFixedWidth(); }
        void setCharacterSize(const unsigned int theSize) { font_size = theSize; calculateFixedWidth(); }
        void setColor(const Color& theColor) { font_color = theColor; }
        void setFixedWidth(const unsigned int theFixedWidth) { fixed_width = theFixedWidth; calculateFixedWidth(); }

};

#endif // _TEXT_HPP_
