#pragma once

#include <optional>
#include <queue>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Window.hpp>
#include "SFML/Graphics/RenderTarget.hpp"

namespace sn {

// o grid posso deixar de tamanho custom

struct Grid {
  int width = 20;
  int height = 20;
};

struct Player {
  sf::Vector2i position = {0, 0};
};

class InputHandler {
 public:
  using KeyboardKey = sf::Keyboard::Key;

  InputHandler() = default;

  inline std::optional<KeyboardKey> GetKeyPressed() {
    std::optional<KeyboardKey> key;
    if (!keys_pressed_.empty()) {
      key.emplace(keys_pressed_.front());
      keys_pressed_.pop();
    }
    return key;
  }

  inline void PushKey(KeyboardKey key) { keys_pressed_.push(key); }

 private:
  std::queue<KeyboardKey> keys_pressed_;
};

class Game {
 public:
  Game(int grid_width, int grid_height) : grid_{grid_width, grid_height} {
    player_ = Player{{grid_width / 2, grid_height / 2}};
  };

  void MovePlayer(sf::Keyboard::Key key_code) {
    using KeyCode = sf::Keyboard::Key;
    switch (key_code) {
      case KeyCode::W:
      case KeyCode::Up: {
        --player_.position.y;
      } break;
      case KeyCode::A:
      case KeyCode::Left: {
        --player_.position.x;
      } break;
      case KeyCode::S:
      case KeyCode::Down: {
        ++player_.position.y;
      } break;
      case KeyCode::D:
      case KeyCode::Right: {
        ++player_.position.x;
      } break;
      default:
        break;
    }
  }

 private:
  Player player_;
  Grid grid_;
};

class Engine {
 public:
  Engine(sf::Window &window, sf::RenderTarget &render_target)
      : window_(window), render_target_(render_target){};

  inline InputHandler &get_input_handler() { return inpute_handler_; }

  void PollEvents();

  void Run();

 private:
  InputHandler inpute_handler_;
  sf::Window &window_;
  sf::RenderTarget &render_target_;
};

}  // namespace sn