#include <iostream>
#include "VideoHandler.h"

void help();

int main() {
    help();
    char* params[] = {"-wte"};
    VideoHandler videoHandler("input/AVG-TownCentre.mp4", 1, params);
    videoHandler.handle();

    return 0;
}

void help() {
    std::cout << "Commands:" << std::endl;
    std::cout << "\tr - change region of interest" << std::endl;
    std::cout << "\tb - show background subtraction process" << std::endl;
    std::cout << "\tc - show contoured image" << std::endl;
    std::cout << "\to - show original video" << std::endl;
    std::cout << "\ts - show system information" << std::endl;
    std::cout << "\tp - pause video" << std::endl;
    std::cout << "\tESC or SPACE - terminate video" << std::endl;
}