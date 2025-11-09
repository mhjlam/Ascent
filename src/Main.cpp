#include "Game/Game.hpp"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
#ifdef _WIN32
    // Set OGRE_CONFIG_DIR to the executable's directory so Ogre loads ogre.cfg from there
    char exe_path[MAX_PATH];
    GetModuleFileNameA(NULL, exe_path, MAX_PATH);
    std::string exe_dir(exe_path);
    size_t last_slash = exe_dir.find_last_of("\\/");
    if (last_slash != std::string::npos) {
        exe_dir = exe_dir.substr(0, last_slash);
    }
    // Add trailing slash as standardisePath expects
    exe_dir += "\\";
    SetEnvironmentVariableA("OGRE_CONFIG_DIR", exe_dir.c_str());
#endif

    Game& game = Game::instance();
    try { 
        game.run();
    } 
    catch (Ogre::Exception& e) {
        std::cerr << "An exception has occurred: " << e.getFullDescription() << '\n';
    }
    
    return 0;
}
