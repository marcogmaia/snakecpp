#pragma once

#include <optional>
#include <queue>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

namespace sn {

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
  Engine() = default;

  inline InputHandler &get_input_handler() { return inpute_handler_; }

private:
  InputHandler inpute_handler_;
};

} // namespace sn