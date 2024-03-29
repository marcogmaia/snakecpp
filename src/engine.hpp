#pragma once

#include <array>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
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

class Rng {
 public:
  Rng()
      : rng_{static_cast<uint32_t>(std::chrono::steady_clock::now().time_since_epoch().count())} {}

  // Get a random number in the closed interval [min, max]
  inline int range(int min, int max) {
    std::uniform_int_distribution<int> udist(min, max);
    return udist(rng_);
  }

  template <typename InputIterator, typename OutputIterator>
  bool random_choice(InputIterator first, InputIterator last, OutputIterator output) {
    // if empty
    if (first == last) {
      return false;
    }
    std::sample(first, last, output, 1, rng_);
    return true;
  }

 private:
  std::mt19937 rng_;
};

class InputHandler {
 public:
  InputHandler() = default;

  inline std::optional<KeyCode> GetKeyPressed() {
    std::optional<KeyCode> key;
    if (KeyHasBeenPressed_()) {
      key.emplace(keys_pressed_.front());
      keys_pressed_.pop();
    }
    return key;
  }

  inline void PushKey(KeyCode key) {
    if (keys_pressed_.size() < 2) {
      keys_pressed_.emplace(key);
    } else {
      keys_pressed_.pop();
      keys_pressed_.emplace(key);
    }
  }

 private:
  std::queue<KeyCode> keys_pressed_;

  inline bool KeyHasBeenPressed_() { return !keys_pressed_.empty(); }
  inline void ResetKeyState_() { keys_pressed_ = std::queue<KeyCode>(); }
};

class Timer {
 public:
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
  using TimeFloat = std::chrono::duration<float>;
  using SClock = std::chrono::steady_clock;

  Timer() { last_time_ = SClock::now(); }

  inline void Init() { last_time_ = SClock::now(); }

  inline float GetDelta() {
    auto now = SClock::now();
    auto delta = now - last_time_;
    last_time_ = now;
    return std::chrono::duration_cast<TimeFloat>(delta).count();
  }

 private:
  // std::chrono::steady_clock sclock_;
  TimePoint last_time_;
};

class Engine {
 public:
  Engine(uint32_t width, uint32_t height)
      : window_{sf::VideoMode{width, height}, game_name, sf::Style::Close | sf::Style::Titlebar},
        render_target_{window_} {
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

  inline void SetWindowTitle(const std::string &title) { window_.setTitle(title); }

 private:
  InputHandler inpute_handler_;
  sf::RenderWindow window_;
  sf::RenderTarget &render_target_;
  Timer timer_;
};

}  // namespace sn