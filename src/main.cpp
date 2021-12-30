#include <spdlog/spdlog.h>

#include "engine.hpp"
#include "game.hpp"

#ifndef DEBUG
// if compiler is MSVC, disable console
#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

int main() {
  spdlog::info("Initializing SFML.");
  sn::Game game(32, 32);
  game.Run();
  return 0;
}
