#pragma once

#include <optional>
#include <queue>
#include <tuple>
#include <vector>

#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Window.hpp>

namespace sn {

// o grid posso deixar de tamanho custom

class Grid {
 public:
  struct Square {
    sf::RectangleShape shape{{8, 8}};
  };

  enum class State {
    kFree,
    kOccupied,
    kFood,
  };

  int width = 20;
  int height = 20;
  static constexpr int npixels_width = 8;
  static constexpr int npixel_height = 8;

  Grid(int w = 20, int h = 20) : width(w), height(h) {
    grid_state_.resize(width * height);
    std::fill(grid_state_.begin(), grid_state_.end(), State::kFree);
    TextureInit_();
  }

  inline State &GetGridPosition(int index) { return grid_state_.at(index); }
  inline State &GetGridPosition(int x, int y) { return GetGridPosition(x + y * width); }

  inline std::tuple<int, int> Index2Coords(int index) {
    int y = index / width;
    int x = index - y * width;
    return std::make_tuple(x, y);
  }

  // Template is a callable, signature: (State state, int index)
  template <typename F>
  void ForeachSquare(F fn) {
    for (size_t index = 0; index < grid_state_.size(); ++index) {
      fn(grid_state_[index], index);
    }
  }

  sf::Sprite GetSprite();

 private:
  std::vector<State> grid_state_;
  sf::RenderTexture render_texture_;

  inline void TextureInit_() {
    auto success = render_texture_.create(width * npixel_height, height * npixel_height);
    if (!success) {
      spdlog::critical("Failed to create SFML texture.");
    }
  }
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

class Engine {
 public:
  Engine(uint32_t width, uint32_t height)
      : window_{sf::VideoMode{width, height}, "Maia"}, render_target_{window_} {};

  inline InputHandler &get_input_handler() { return inpute_handler_; }

  void PollEvents();

  // Render a renderable / drawable
  void Render();

  void Display();

  void Run();

 private:
  InputHandler inpute_handler_;
  sf::RenderWindow window_;
  sf::RenderTarget &render_target_;
};

class Game {
 public:
  Game(int grid_width, int grid_height) : grid_{grid_width, grid_height} {
    player_ = Player{{grid_width / 2, grid_height / 2}};
  };

  void MovePlayer(sf::Keyboard::Key key_code);

  inline Grid &get_grid() { return grid_; }

  void Run();

 private:
  Player player_;
  Grid grid_;
  Engine engine_{800, 600};
};

}  // namespace sn