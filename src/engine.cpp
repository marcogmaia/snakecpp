
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

// TODO move this from here
void Engine::Run() {
  // Grid& grid = game_.get_grid();

  while (window_.isOpen()) {
    PollEvents();
    render_target_.clear(sf::Color::Black);
    // auto shape = grid.GetSprite();
    // shape.setPosition({64, 64});
    // render_target_.draw(shape);
    window_.display();
  }
}

void Game::MovePlayer(sf::Keyboard::Key key_code) {
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

void Game::Run() {
  grid_.GetGridPosition(3, 3) = Grid::State::kFood;
  grid_.GetGridPosition(4, 3) = Grid::State::kOccupied;
  grid_.GetGridPosition(8, 9) = Grid::State::kOccupied;
  
  // TODO create while loop while game is running
  engine_.PollEvents();
  engine_.Render();
  engine_.Display();

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

}  // namespace sn