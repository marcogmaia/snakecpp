#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "engine.hpp"
#include "game.hpp"

#ifndef DEBUG
#ifdef _WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
#endif

int main() {
  spdlog::info("Initializing SFML.");
  sn::Game game(32, 32);
  game.Run();
  return 0;
}
