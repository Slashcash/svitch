#ifndef _MAINSTATE_HPP_
#define _MAINSTATE_HPP_

#include <vector>

#include "state.hpp"
#include "text.hpp"
#include "savefile.hpp"

/*the principal state of this software*/

class MainState : public State {
    private:
        std::vector<SaveFile> savefiles; //all the savefiles in the system
        unsigned int save_selected;

        Text game_title;
        Text game_author;
        Text game_userid;

        Text arrow_info;
        Text export_info;
        Text import_info;
        Text exit_info;

        Text app_title;

        void buildTitleInfo();
        void export_svi();
        void import_svi();

        std::string chooseExportFileName() const;

    public:
        MainState();

        virtual void onNotify(const Signal& theSignal);
};

#endif //_MAINSTATE_HPP_
