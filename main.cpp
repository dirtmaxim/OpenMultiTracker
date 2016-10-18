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
    std::cout << "r - change region of interest" << endl;
}