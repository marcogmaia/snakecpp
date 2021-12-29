#include "game.hpp"

namespace sn {

void Game::SetPlayerMovementDirection(KeyCode key_code) {
  // clang-format off
  switch (key_code) {
    case KeyCode::W: [[fallthrough]];
    case KeyCode::Up: {
      player_.set_movement_direction(Player::MovementDirection::kUp);
    } break;
    case KeyCode::A: [[fallthrough]];
    case KeyCode::Left: {
      player_.set_movement_direction(Player::MovementDirection::kLeft);
    } break;
    case KeyCode::S: [[fallthrough]];
    case KeyCode::Down: {
      player_.set_movement_direction(Player::MovementDirection::kDown);
    } break;
    case KeyCode::D: [[fallthrough]];
    case KeyCode::Right: {
      player_.set_movement_direction(Player::MovementDirection::kRight);
    } break;
    default:
      break;
  }
  // clang-format on
}

void Game::Run() {
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
  render_texture_.clear(sf::Color::Black);
  // Render texture with what is shown on the grid, and the return a sprite of the grid
  ForeachSquare(fn);
  sf::Sprite sp{render_texture_.getTexture()};
  return sp;
}

void Game::ProcessTurn(float elapsed_time) {
  // if completed turn time, get the movement direction and move the snake
  // TODO need to define the *turn_time*
  constexpr float khack_turn_time = .1;
  static float time_bucket = 0.f;
  time_bucket += elapsed_time;
  if (time_bucket < khack_turn_time) {
    return;
  }
  time_bucket -= khack_turn_time;
  // do logic here
  auto [x, y] = player_.GetHeadPosition();
  switch (player_.get_movement_direction()) {
    // clang-format off
    case Player::MovementDirection::kUp:    ++y; break;
    case Player::MovementDirection::kDown:  --y; break;
    case Player::MovementDirection::kLeft:  --x; break;
    case Player::MovementDirection::kRight: ++x; break;
    default: break;
      // clang-format on
  }
  player_.Move({x, y});
  // TODO:
  // 1 - Check for food
  // 2 - Grow snake
  // 3 - Check losing conditions
  grid_.Clear();
  UpdateGridFromPlayer();
}

void Player::Move(sf::Vector2i new_pos) {
  new_pos = LimitWrap_(new_pos);
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

sf::Vector2i Player::LimitWrap_(sf::Vector2i pos) {
  if (pos.x >= limits_.x) {
    pos.x = 0;
  } else if (pos.x < 0) {
    pos.x = limits_.x - 1;
  }
  if (pos.y >= limits_.y) {
    pos.y = 0;
  } else if (pos.y < 0) {
    pos.y = limits_.y - 1;
  }
  return pos;
}

}  // namespace sn