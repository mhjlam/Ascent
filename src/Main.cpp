#include "Game/Game.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
#ifdef _WIN32
    // Set OGRE_CONFIG_DIR to the executable's directory so Ogre loads ogre.cfg from there
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);
    std::string exeDir(exePath);
    size_t lastSlash = exeDir.find_last_of("\\/");
    if (lastSlash != std::string::npos) {
        exeDir = exeDir.substr(0, lastSlash);
    }
    // Add trailing slash as standardisePath expects
    exeDir += "\\";
    SetEnvironmentVariableA("OGRE_CONFIG_DIR", exeDir.c_str());
#endif

    Game* game = Game::instance(); // create main game class

    try { 
        game->run();
    } 
    catch (Ogre::Exception& e) {
        std::cerr << "An exception has occurred: " << e.getFullDescription() << std::endl;
    }
    
    return 0;
}
