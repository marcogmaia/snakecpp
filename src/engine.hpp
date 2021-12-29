#pragma once

#include <numeric>
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

using KeyCode = sf::Keyboard::Key;
constexpr char const *game_name = "Snake";

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

class Player {
 public:
  enum class MovementDirection {
    kUp,
    kDown,
    kLeft,
    kRight,
  };

  Player(int x, int y) {
    // initial player position
    positions_.emplace_front(x, y);
  }

  // moves the entire body of the snake
  void Move(sf::Vector2i new_pos) {
    auto &head = positions_.front();
    auto &tail = positions_.back();
    bool body_exists = &head != &tail;
    if (body_exists) {
      positions_.emplace_front(new_pos);
      positions_.pop_back();
    } else {
      head = new_pos;
    }
  }

  void SetMovementDirection(MovementDirection direction) {}

  void Grow(sf::Vector2i pos) {
    // add new segment in this position
    positions_.push_front(pos);
  }

  inline sf::Vector2i GetHeadPosition() { return positions_.front(); }

 private:
  std::deque<sf::Vector2i> positions_;
};

class InputHandler {
 public:
  InputHandler() = default;

  inline std::optional<KeyCode> GetKeyPressed() {
    std::optional<KeyCode> key;
    if (!keys_pressed_.empty()) {
      key.emplace(keys_pressed_.front());
      keys_pressed_.pop();
    }
    return key;
  }

  inline void PushKey(KeyCode key) { keys_pressed_.push(key); }

 private:
  std::queue<KeyCode> keys_pressed_;
};

class Timer {
 public:
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
  using TimeFloat = std::chrono::duration<float>;

  Timer() { last_time_ = sclock_.now(); }

  inline void Init() { last_time_ = sclock_.now(); }

  inline float GetDelta() {
    using namespace std::chrono;
    auto now = sclock_.now();
    auto delta = now - last_time_;
    last_time_ = now;
    return duration_cast<TimeFloat>(delta).count();
  }

 private:
  std::chrono::steady_clock sclock_;
  TimePoint last_time_;
};

class Engine {
 public:
  Engine(uint32_t width, uint32_t height)
      : window_{sf::VideoMode{width, height}, game_name}, render_target_{window_} {};

  inline InputHandler &get_input_handler() { return inpute_handler_; }

  void PollEvents();

  // Render a renderable / drawable
  inline void Render(const sf::Drawable &drawable) {
    render_target_.clear();
    render_target_.draw(drawable);
  }

  inline void Display() { window_.display(); }

  inline Timer &get_timer() { return timer_; }

  bool ShouldTerminate() { return !window_.isOpen(); }

 private:
  InputHandler inpute_handler_;
  sf::RenderWindow window_;
  sf::RenderTarget &render_target_;
  Timer timer_;
};

class Game {
 public:
  Game(int grid_width, int grid_height)
      : grid_{grid_width, grid_height},
        player_{grid_width / 2, grid_height / 2},
        engine_{static_cast<uint32_t>(Grid::npixels_width * grid_width),
                static_cast<uint32_t>(Grid::npixel_height * grid_height)} {};

  void SetPlayerMovementDirection(sf::Keyboard::Key key_code);

  void ProcessTurn(float elapsed_time);

  void Run();

  inline void CheckRunningStatus() { is_running_ = !engine_.ShouldTerminate(); }

 private:
  Grid grid_;
  Player player_;
  Engine engine_;
  bool is_running_ = true;
};

}  // namespace sn