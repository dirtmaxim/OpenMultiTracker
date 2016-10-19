#include <iostream>
#include "VideoHandler.h"

void help();

int main() {
    help();
    VideoHandler videoHandler("hockey_game.avi");
    videoHandler.handle();
    return 0;
}

void help() {
    std::cout << "Commands:" << endl;
    std::cout << "\tr - change region of interest" << endl;
    std::cout << "\tb - show background subtraction process" << endl;
    std::cout << "\tc - show contoured image" << endl;
    std::cout << "\to - show original video" << endl;
}