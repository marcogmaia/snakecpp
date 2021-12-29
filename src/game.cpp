#include "game.hpp"

namespace sn {

void Game::SetPlayerMovementDirection(KeyCode key_code) {
  // BUG: quando se pressiona as teclas rápido o suficiente, pode andar na direção contrária
  Player::MovementDirection new_direction = Player::MovementDirection::kRight;
  // clang-format off
  switch (key_code) {
    case KeyCode::W: [[fallthrough]];
    case KeyCode::Up: new_direction = Player::MovementDirection::kUp; break;
    case KeyCode::A: [[fallthrough]];
    case KeyCode::Left: new_direction = Player::MovementDirection::kLeft; break;
    case KeyCode::S: [[fallthrough]];
    case KeyCode::Down: new_direction = Player::MovementDirection::kDown; break;
    case KeyCode::D: [[fallthrough]];
    case KeyCode::Right: new_direction = Player::MovementDirection::kRight; break;
    default: break;
  }
  // clang-format on
  if (!player_.IsDirectionBackwards(new_direction)) {
    player_.set_movement_direction(new_direction);
  }
}

void Game::Run() {
  Timer &timer = engine_.get_timer();
  timer.Init();

  while (is_running_) {
    engine_.PollEvents();
    // Game logic
    ProcessTurn(timer.GetDelta());
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
  // TODO need to define the *turn_time* as an exponential funtion, limited by fps
  constexpr float khack_turn_time = .1;
  constexpr float size_speed_multiplier = 0.002;
  float turn_time = khack_turn_time - (player_.Size() * size_speed_multiplier);
  static float time_bucket = 0.f;
  time_bucket += elapsed_time;
  if (time_bucket < turn_time) {
    return;
  }
  time_bucket -= turn_time;
  // consumes direction key
  UpdatePlayerMovementDirection_();
  auto next_move_pos = GetPlayerNextPosition_();
  // check losing condition
  // if next_pos is occupied, player loses, restart game
  game_over_ = grid_.GetGridTile(next_move_pos) == Grid::State::kOccupied;
  if (game_over_) {
    // restart game
    NewGame();
    return;
  }

  // 1 - Check for food
  bool tile_has_food = grid_.GetGridTile(next_move_pos) == Grid::State::kFood;
  if (tile_has_food) {
    // 2 - Grow snake
    player_.Grow(next_move_pos);
    // remove food from map
    food_manager_.CreateFood(GetGridFreeIndexes_());
  } else {
    player_.Move(next_move_pos);
  }

  // 3 - Check losing conditions
  grid_.Clear();
  UpdateGridFromPlayer();
}

void Player::Move(sf::Vector2i new_pos) {
  new_pos = WrapLimits_(new_pos);
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

sf::Vector2i Player::WrapLimits_(sf::Vector2i pos) {
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

std::vector<int> Game::GetGridFreeIndexes_() {
  std::vector<int> free_indexes;
  free_indexes.reserve(grid_.Size());
  auto fn = [&free_indexes](Grid::State state, int index) {
    if (state == Grid::State::kFree) {
      free_indexes.emplace_back(index);
    }
  };
  grid_.ForeachSquare(fn);
  return free_indexes;
}

void Game::UpdatePlayerMovementDirection_() {
  InputHandler &input_handler = engine_.get_input_handler();
  auto key_pressed = input_handler.GetKeyPressed();
  if (key_pressed) {
    SetPlayerMovementDirection(*key_pressed);
  }
}

sf::Vector2i Game::GetPlayerNextPosition_() {
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
  // wraps if out of grid
  sf::Vector2i next_move_pos = player_.GetMoveTilePos({x, y});
  return next_move_pos;
}
}  // namespace sn