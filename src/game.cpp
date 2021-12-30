#include "game.hpp"

#include <cmath>

namespace sn {

void Game::SetPlayerMovementDirection(KeyCode key_code) {
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
    CheckRunningStatus_();
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

namespace {

constexpr float GetTurnTime(int size) {
  constexpr float initial_turn_time = 1.f / 10.f;
  constexpr float base_time = 1.f / 60.f;
  float interp = std::lerp(0.f, 2.f, (size - 1) / 100.f);
  float turn_time = ((initial_turn_time - base_time) * std::expf(-interp)) + base_time;
  return turn_time;
}

}  // namespace

void Game::ProcessTurn(float elapsed_time) {
  // if completed turn time, get the movement direction and move the snake
  float turn_time = GetTurnTime(player_.Size());
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
  // FIXME: game over condition is to check if any part of the body occupies the same square as Head
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
  auto wrap_limit = [](int actual, int limit) -> int {
    int ret = actual;
    if (actual >= limit) {
      ret = 0;
    } else if (actual < 0) {
      ret = limit - 1;
    }
    return ret;
  };
  pos.x = wrap_limit(pos.x, limits_.x);
  pos.y = wrap_limit(pos.y, limits_.y);
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

void Game::NewGame() {
  game_over_ = false;
  player_ = Player({grid_.width / 2, grid_.height / 2}, {grid_.width, grid_.height});
  CreateFood();
  grid_.GetGridTile(food_manager_.get_food_index()) = Grid::State::kFood;
  UpdateGridFromPlayer();
}

void Game::UpdateGridFromPlayer() {
  grid_.Clear();
  for (const auto &pos : player_.get_positions()) {
    grid_.GetGridTile(pos) = Grid::State::kOccupied;
  }
  grid_.GetGridTile(food_manager_.get_food_index()) = Grid::State::kFood;
}

}  // namespace sn