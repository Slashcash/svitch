#ifndef _IMPORTSTATE_HPP_
#define _IMPORTSTATE_HPP_

#include <vector>

#include "state.hpp"
#include "savefile.hpp"
#include "opresult.hpp"
#include "text.hpp"
#include "svifile.hpp"

/*the import state of this software*/

class ImportState : public State {
    private:
        SaveFile savefile; //the savefile we are importing into
        std::vector<SVIFile> savefiles_folder; //all the .svi files we find in the svitch folder
        unsigned int save_selected;

        Text importing_for;
        Text no_importing;

        Text file_found;
        Text file_info;
        Text game_title;
        Text game_author;

        Text import_info;
        Text arrow_info;
        Text exit_info;

        void buildTitleInfo();
        void import_svi();

    public:
        ImportState(const SaveFile& theSaveFile);

        virtual void onNotify(const Signal& theSignal);
};

#endif //_IMPORTSTATE_HPP_
