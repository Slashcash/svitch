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
        std::vector<SaveFile> savefiles; //all the savefiles in the system
        #ifndef EMULATOR
        std::vector<Account> system_accounts;
        #endif
        unsigned int save_selected;
        unsigned int carousel_page;
        unsigned int users_page;

        std::vector<Sprite> cover_arts;
        std::vector<Text> cover_names;
        std::vector<Text> cover_authors;
        std::vector<Sprite> user_arts;
        std::vector<Text> user_names;
        Sprite selector_sprite;


        void buildScreen();
        void buildCarousel();
        void buildUsers();
        void buildSelector();
        void buildCarouselPage(const unsigned int thePage);
        void buildUsersPage(const unsigned int thePage);

        void changeSelected(unsigned int theSelection);
        std::string chooseExportFileName() const;

    public:
        MainState(const Size theSceneSize);

        virtual void onNotify(const Signal& theSignal);
};

#endif //_MAINSTATE_HPP_
