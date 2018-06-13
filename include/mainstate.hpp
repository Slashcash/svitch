#ifndef _MAINSTATE_HPP_
#define _MAINSTATE_HPP_

#include <vector>

#include "state.hpp"
#include "text.hpp"
#include "savefile.hpp"
#include "sprite.hpp"
#include "logwriter.hpp"

/*the principal state of this software*/

class MainState : public State, public LogWriter {
    private:
        static const int FRAME_TO_FRAME_SPACING = 20;

        std::vector<SaveFile> savefiles; //all the savefiles in the system
        unsigned int save_selected;
        unsigned int carousel_page;

        std::vector<Sprite> cover_arts;
        Text game_title;
        Text game_author;
        Text game_user;

        void buildCarousel();
        void buildTitleInfo();
        void buildScreen();
        void buildPage();

        void changeSelected(unsigned int theSelection);
        std::string chooseExportFileName() const;

    public:
        MainState();

        virtual void onNotify(const Signal& theSignal);
};

#endif //_MAINSTATE_HPP_
