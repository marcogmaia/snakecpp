#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "engine.hpp"
#include "game.hpp"

int main() {
  spdlog::info("Initializing SFML.");
  sn::Game game(32, 32);
  game.Run();
  return 0;
}
