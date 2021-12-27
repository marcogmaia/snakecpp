
#include "engine.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

namespace sn {

namespace {}

void Engine::PollEvents() {
  sf::Event event;
  while (window_.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::Closed: {
        window_.close();
      } break;
      case sf::Event::Resized:
        break;
      case sf::Event::LostFocus:
        break;
      case sf::Event::GainedFocus:
        break;
      case sf::Event::TextEntered:
        break;
      case sf::Event::KeyPressed: {
        get_input_handler().PushKey(event.key.code);
      } break;
      case sf::Event::KeyReleased:
        break;
      case sf::Event::MouseWheelScrolled:
        break;
      case sf::Event::MouseButtonPressed:
        break;
      case sf::Event::MouseButtonReleased:
        break;
      case sf::Event::MouseMoved:
        break;
      case sf::Event::MouseEntered:
        break;
      case sf::Event::MouseLeft:
        break;
      case sf::Event::JoystickButtonPressed:
        break;
      case sf::Event::JoystickButtonReleased:
        break;
      case sf::Event::JoystickMoved:
        break;
      case sf::Event::JoystickConnected:
        break;
      case sf::Event::JoystickDisconnected:
        break;
      case sf::Event::TouchBegan:
        break;
      case sf::Event::TouchMoved:
        break;
      case sf::Event::TouchEnded:
        break;
      case sf::Event::SensorChanged:
        break;
      case sf::Event::Count:
        break;
      default:
        break;
    }
  }
}

void Engine::Run() {
  while (window_.isOpen()) {
    PollEvents();
    render_target_.clear(sf::Color::Black);
    window_.display();
  }
}
}  // namespace sn