#pragma once

#include <span>

#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>

#include "engine.hpp"

namespace sn {

class FoodManager {
 public:
  explicit FoodManager(std::span<const int> free_spots) { CreateFood(free_spots); }

  inline void CreateFood(std::span<const int> free_spots) {
    rng_.random_choice(free_spots.begin(), free_spots.end(), &food_index_);
  }

  [[nodiscard]] inline int get_food_index() const { return food_index_; }

 private:
  int food_index_ = 0;
  Rng rng_;
};

class Player {
 public:
  enum class MovementDirection {
    kUp,
    kDown,
    kLeft,
    kRight,
  };

  Player(sf::Vector2i initial_pos, sf::Vector2i limits) : limits_{limits} {
    // initial player position
    positions_.emplace_front(initial_pos);
  }

  // moves the entire body of the snake
  void Move(sf::Vector2i new_pos);

  inline void set_movement_direction(MovementDirection direction) {
    movement_direction_ = direction;
  }

  [[nodiscard]] inline MovementDirection get_movement_direction() const {
    return movement_direction_;
  }

  sf::Vector2i GetMoveTilePos(sf::Vector2i pos) { return WrapLimits_(pos); }

  inline void Grow(sf::Vector2i pos) { positions_.emplace_front(pos); }

  [[nodiscard]] inline size_t Size() const { return positions_.size(); }

  inline sf::Vector2i GetHeadPosition() { return positions_.front(); }

  [[nodiscard]] inline const std::deque<sf::Vector2i> &get_positions() const { return positions_; }

  bool HeadOverlapsWithBody();

  bool IsDirectionBackwards(MovementDirection direction) {
    bool is_backwards = false;
    // clang-format off
    switch (direction) {
     case MovementDirection::kUp:    is_backwards = (movement_direction_ == MovementDirection::kDown);  break;
     case MovementDirection::kDown:  is_backwards = (movement_direction_ == MovementDirection::kUp);    break;
     case MovementDirection::kLeft:  is_backwards = (movement_direction_ == MovementDirection::kRight); break;
     case MovementDirection::kRight: is_backwards = (movement_direction_ == MovementDirection::kLeft);  break;
     default: break;
    }
    // clang-format on
    return is_backwards;
  }

 private:
  std::deque<sf::Vector2i> positions_;
  MovementDirection movement_direction_ = MovementDirection::kRight;
  sf::Vector2i limits_;

  [[nodiscard]] sf::Vector2i WrapLimits_(sf::Vector2i pos) const;
};

class Grid {
 public:
  enum class State {
    kFree,
    kOccupied,
    kFood,
  };

  static constexpr int npixels_width = 8;
  static constexpr int npixel_height = 8;

  explicit Grid(int w = 20, int h = 20) : width_(w), height_(h) {
    grid_state_.resize(width_ * height_);
    std::fill(grid_state_.begin(), grid_state_.end(), State::kFree);
    TextureInit_();
  }

  inline State &GetGridTile(int index) { return grid_state_.at(index); }
  inline State &GetGridTile(int x, int y) { return GetGridTile(x + y * width_); }
  inline State &GetGridTile(sf::Vector2i pos) { return GetGridTile(pos.x, pos.y); }

  inline std::tuple<int, int> Index2Coords(int index) const {
    int y = index / width_;
    int x = index - y * width_;
    return std::make_tuple(x, y);
  }

  // Template is a callable, signature: (State state, int index)
  template <typename F>
  void ForeachSquare(F fn) {
    for (size_t index = 0; index < grid_state_.size(); ++index) {
      fn(grid_state_[index], index);
    }
  }

  sf::Sprite GetSprite();

  // Clears grid states, set all to kFree.
  inline void Clear() { std::fill(std::begin(grid_state_), std::end(grid_state_), State::kFree); }

  inline size_t Size() { return grid_state_.size(); }

  inline int get_width() const { return width_; }
  inline int get_height() const { return height_; }

 private:
  std::vector<State> grid_state_;
  sf::RenderTexture render_texture_;
  int width_ = 20;
  int height_ = 20;

  inline void TextureInit_() {
    auto success = render_texture_.create(width_ * npixel_height, height_ * npixel_height);
    if (!success) {
      spdlog::critical("Failed to create SFML texture.");
    }
  }
};

class Game {
 public:
  Game(int grid_width, int grid_height)
      : grid_{grid_width, grid_height},
        player_{{grid_width / 2, grid_height / 2}, {grid_width, grid_height}},
        engine_{static_cast<uint32_t>(Grid::npixels_width * grid_width),
                static_cast<uint32_t>(Grid::npixel_height * grid_height)},
        food_manager_{GetGridFreeIndexes_()} {
    NewGame();
  };

  void SetPlayerMovementDirection(sf::Keyboard::Key key_code);

  void ProcessTurn(float elapsed_time);

  void Run();

  void UpdateGridFromPlayer();

  inline void CreateFood() {
    auto free_indexes = GetGridFreeIndexes_();
    food_manager_.CreateFood(free_indexes);
    grid_.GetGridTile(food_manager_.get_food_index()) = Grid::State::kFood;
  }

  void NewGame();

  template <typename T>
  inline void ShowScore(T score) {
    engine_.SetWindowTitle(fmt::format("Snake - score: {}", player_.Size()));
  }

 private:
  Grid grid_;
  Player player_;
  Engine engine_;
  FoodManager food_manager_;
  bool game_over_ = false;
  bool is_running_ = true;

  std::vector<int> GetGridFreeIndexes_();
  void UpdatePlayerMovementDirection_();
  sf::Vector2i GetPlayerNextPosition_();
  inline void CheckRunningStatus_() { is_running_ = !engine_.ShouldTerminate(); }
};

}  // namespace sn