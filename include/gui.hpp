#ifndef _GUI_HPP_
#define _GUI_HPP_

/*The main gui class for this library*/

#include <stack>
#include <vector>

#include "logwriter.hpp"
#include "texture.hpp"
#include "sprite.hpp"
#include "font.hpp"
#include "text.hpp"

class State;

class Gui : public LogWriter {
    private:
        static const int APPTITLE_Y_POSITION = 25;
        static const int APPTITLE_SPACING = 5;

        static Gui* instance;
        static bool is_initialized;
        std::stack<State*> states;
        std::vector<std::string> supported_languages; //all the languages supported by this software, see http://switchbrew.org/index.php?title=Settings_services#LanguageCode for codes
        s32 system_language; //the system language set on the switch
        std::string system_language_code; //its string representation

        //SOME GRAPHICAL
        Texture background_texture;
        Sprite background_sprite; //the background
        Font apptitle_font;
        Text apptitle_text;

        Gui();

        void dropState();
        void shutdown();
        OPResult getSetLanguage();

    public:
        Gui(const Gui& theGui) = delete;
        Gui& operator=(const Gui& theGui) = delete;

        void addState(State* theState); //theState should be on the heap

        void run();
        static Gui* getInstance();

        s32 getSystemLanguage() const { return system_language; }
        std::string getSystemLanguageCode() const { return system_language_code; }
        static void destroy(); //needed because this is a singleton
};

#endif // _GUI_HPP_
