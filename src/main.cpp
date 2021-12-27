#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "engine.hpp"

int main() {
  spdlog::info("Initializing SFML.");
  sf::RenderWindow window(sf::VideoMode(800, 600), "Maia");
  sn::Engine engine{window, window};
  engine.Run();
  return 0;
}