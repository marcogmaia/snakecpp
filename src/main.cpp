#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "engine.hpp"

int main() {
  spdlog::info("Initializing SFML.");
  // window.setFramerateLimit(120);
  // sn::Engine engine{window, window};
  sn::Game game(32, 32);
  // engine.Run();
  game.Run();
  return 0;
}
