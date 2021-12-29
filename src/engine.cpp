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

void Game::SetPlayerMovementDirection(KeyCode key_code) {
  switch (key_code) {
    case KeyCode::W:
    case KeyCode::Up: {
      player_.SetMovementDirection(Player::MovementDirection::kUp);
    } break;
    case KeyCode::A:
    case KeyCode::Left: {
      player_.SetMovementDirection(Player::MovementDirection::kLeft);
    } break;
    case KeyCode::S:
    case KeyCode::Down: {
      player_.SetMovementDirection(Player::MovementDirection::kDown);
    } break;
    case KeyCode::D:
    case KeyCode::Right: {
      player_.SetMovementDirection(Player::MovementDirection::kRight);
    } break;
    default:
      break;
  }
}

void Game::Run() {
  grid_.GetGridPosition(3, 3) = Grid::State::kFood;
  grid_.GetGridPosition(4, 3) = Grid::State::kOccupied;
  grid_.GetGridPosition(8, 9) = Grid::State::kOccupied;

  Timer &timer = engine_.get_timer();
  timer.Init();
  InputHandler &input_handler = engine_.get_input_handler();

  while (is_running_) {
    engine_.PollEvents();
    // Game logic
    {
      auto key_pressed = input_handler.GetKeyPressed();
      if (key_pressed) {
        SetPlayerMovementDirection(*key_pressed);
      }
      ProcessTurn(timer.GetDelta());
    }
    engine_.Render(grid_.GetSprite());
    engine_.Display();
    CheckRunningStatus();
  }
}

sf::Sprite Grid::GetSprite() {
  auto fn = [this](State s, int index) {
    auto [x, y] = Index2Coords(index);
    sf::RectangleShape rshape{{npixels_width, npixel_height}};
    rshape.setPosition({
        static_cast<float>(npixels_width * x),
        static_cast<float>(npixel_height * y),
    });
    switch (s) {
      case State::kFree:
        rshape.setFillColor(sf::Color::Black);
        break;
      case State::kOccupied:
        rshape.setFillColor(sf::Color::White);
        break;
      case State::kFood:
        rshape.setFillColor(sf::Color::Green);
        break;
      default:
        break;
    }
    render_texture_.draw(rshape);
  };
  // Clear the texture
  render_texture_.clear(sf::Color::Black);
  // Render texture with what is shown on the grid, and the return a sprite of the grid
  ForeachSquare(fn);
  sf::Sprite sp{render_texture_.getTexture()};
  return sp;
}

void Game::ProcessTurn(float elapsed_time) {
  // if completed turn time, get the movement direction and move the snake
  // TODO need to define the *turn_time*
}

}  // namespace sn