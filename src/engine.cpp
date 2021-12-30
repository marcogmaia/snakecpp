#include <chrono>

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include "engine.hpp"

namespace sn {

namespace {}

void Engine::PollEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed: {
        window_.close();
      } break;
      case sf::Event::KeyPressed: {
        get_input_handler().PushKey(event.key.code);
      } break;
      case sf::Event::Resized:
      case sf::Event::LostFocus:
      case sf::Event::GainedFocus:
      case sf::Event::TextEntered:
      case sf::Event::KeyReleased:
      case sf::Event::MouseWheelScrolled:
      case sf::Event::MouseButtonPressed:
      case sf::Event::MouseButtonReleased:
      case sf::Event::MouseMoved:
      case sf::Event::MouseEntered:
      case sf::Event::MouseLeft:
      case sf::Event::JoystickButtonPressed:
      case sf::Event::JoystickButtonReleased:
      case sf::Event::JoystickMoved:
      case sf::Event::JoystickConnected:
      case sf::Event::JoystickDisconnected:
      case sf::Event::TouchBegan:
      case sf::Event::TouchMoved:
      case sf::Event::TouchEnded:
      case sf::Event::SensorChanged:
      case sf::Event::Count:
      default:
        break;
    }
  }
}

}  // namespace sn