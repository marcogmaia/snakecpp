#pragma once

#include <spdlog/spdlog.h>
#include <SFML/Graphics.hpp>

#include "engine.hpp"

namespace sn {

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

  inline MovementDirection get_movement_direction() const { return movement_direction_; }

  void Grow(sf::Vector2i pos) {
    // add new segment in this position
    positions_.push_front(pos);
  }

  inline sf::Vector2i GetHeadPosition() { return positions_.front(); }

  inline const std::deque<sf::Vector2i> &get_positions() const { return positions_; }

 private:
  std::deque<sf::Vector2i> positions_;
  MovementDirection movement_direction_ = MovementDirection::kRight;
  sf::Vector2i limits_;

  sf::Vector2i LimitWrap_(sf::Vector2i pos);
};

class Grid {
 public:
  enum class State {
    kFree,
    kOccupied,
    kFood,
  };

  int width = 20;
  int height = 20;
  static constexpr int npixels_width = 8;
  static constexpr int npixel_height = 8;

  Grid(int w = 20, int h = 20) : width(w), height(h) {
    grid_state_.resize(width * height);
    std::fill(grid_state_.begin(), grid_state_.end(), State::kFree);
    TextureInit_();
  }

  inline State &GetGridTile(int index) { return grid_state_.at(index); }
  inline State &GetGridTile(int x, int y) { return GetGridTile(x + y * width); }
  inline State &GetGridTile(sf::Vector2i pos) { return GetGridTile(pos.x, pos.y); }

  inline std::tuple<int, int> Index2Coords(int index) {
    int y = index / width;
    int x = index - y * width;
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

 private:
  std::vector<State> grid_state_;
  sf::RenderTexture render_texture_;

  inline void TextureInit_() {
    auto success = render_texture_.create(width * npixel_height, height * npixel_height);
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
                static_cast<uint32_t>(Grid::npixel_height * grid_height)} {
    UpdateGridFromPlayer();
  };

  ~Game() = default;

  void SetPlayerMovementDirection(sf::Keyboard::Key key_code);

  void ProcessTurn(float elapsed_time);

  void Run();

  inline void CheckRunningStatus() { is_running_ = !engine_.ShouldTerminate(); }

  inline void UpdateGridFromPlayer() {
    grid_.Clear();
    for (const auto &pos : player_.get_positions()) {
      grid_.GetGridTile(pos) = Grid::State::kOccupied;
    }
  }

 private:
  Grid grid_;
  Player player_;
  Engine engine_;
  bool is_running_ = true;
};
}  // namespace sn