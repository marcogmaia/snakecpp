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
      : window_{sf::VideoMode{width, height}, game_name}, render_target_{window_} {
    window_.setFramerateLimit(120);
  };

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

}  // namespace sn