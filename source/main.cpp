#include "gui.hpp"

int main() {
    Gui::getInstance()->run();
    Gui::destroy();
}
