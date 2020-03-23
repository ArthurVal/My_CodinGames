#include <cstdlib>  // size_t

#include <cstdint>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <tuple>    // std::tie
#include <utility>  // pair
#include <vector>

#include <algorithm>

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

typedef int player_id_t;

///////////////////////////////////////////////////////////////////////////////
//                                   UTILS                                   //
///////////////////////////////////////////////////////////////////////////////

namespace utils {
namespace ascii {

inline std::string horizontalBar(const std::size_t &length) noexcept {
  std::stringstream ss;
  for (auto i = 0; i < length; ++i) ss << "-";
  return ss.str();
};

}  // namespace ascii
}  // namespace utils

///////////////////////////////////////////////////////////////////////////////
//                                   OCEAN                                   //
///////////////////////////////////////////////////////////////////////////////
namespace ocean {

// OCEAN TYPES ////////////////////////////////////////////////////////////////
#define OCEAN_TILE_MASK 0b0000011
#define OCEAN_TILE_FREE 0b0000001
#define OCEAN_TILE_ISLE 0b0000010

#define OCEAN_TILE_PLAYER_MASK 0b0001100
#define OCEAN_TILE_VISITED 0b0000100
#define OCEAN_TILE_TARGET 0b0001000

typedef std::uint32_t cell_t;
typedef std::vector<cell_t> line_t;
typedef std::vector<line_t> map_t;

struct Position {
  int x, y;
};
typedef std::vector<Position> path_t;
typedef std::list<path_t> path_list_t;

// OCEAN STR FORMAT ///////////////////////////////////////////////////////////
inline const std::string draw(const cell_t &cell) noexcept {
  switch (cell) {
    case OCEAN_TILE_FREE:
      return std::string(" ");
    case OCEAN_TILE_FREE | OCEAN_TILE_VISITED:
      return std::string("O");
    case OCEAN_TILE_ISLE:
      return std::string("X");
    case OCEAN_TILE_FREE | OCEAN_TILE_TARGET:
      return std::string("T");
    default:
      return std::string("?");
  }
};

inline std::string draw(const line_t &line) noexcept {
  std::stringstream output;
  for (const auto &cell : line) output << draw(cell);
  return output.str();
};

inline std::string draw(const map_t &map) noexcept {
  std::stringstream output;
  output << "Current Map (" << map.size() << "x" << map[0].size() << "):\n";
  output << "*" << utils::ascii::horizontalBar(map.size()) << "*\n";
  for (const auto &horizontal_line : map)
    output << "|" << draw(horizontal_line) << "|\n";

  output << "*" << utils::ascii::horizontalBar(map.size()) << "*\n";

  return output.str();
};

std::ostream &operator<<(std::ostream &output, const map_t &map) {
  output << draw(map);
  return output;
};

// OCEAN FUNCTIONS ////////////////////////////////////////////////////////////
void clean(map_t &map, const cell_t &mask) {
  for (auto &line : map)
    for (auto &cell : line) cell &= ~mask;
}

}  // namespace ocean

///////////////////////////////////////////////////////////////////////////////
//                                SUBMARINE                                  //
///////////////////////////////////////////////////////////////////////////////
namespace submarine {

// PLAYER TYPES ///////////////////////////////////////////////////////////////

struct Player {
  ocean::Position pos;
  int hp;
  int torpedo_cd;
  int sonar_cd;
  int silence_cd;
  int mine_cd;
};

namespace action {

enum Type { movement, torpedo, surface, msg };

namespace move {
enum Direction : char { North = 'N', South = 'S', East = 'E', West = 'W' };

enum Power {
  none,
  torpedo,
};
}  // namespace move

namespace str {

std::string to_string(const Type &action) {
  switch (action) {
    case Type::movement:
      return std::string("MOVE");
    case Type::torpedo:
      return std::string("TORPEDO");
    case Type::surface:
      return std::string("SURFACE");
    case Type::msg:
      return std::string("MSG");
    default:
      return std::string("");
  };
};

std::string to_string(const move::Power &powa) {
  switch (powa) {
    case move::Power::torpedo:
      return std::string("TORPEDO");
    case move::Power::none:
    default:
      return std::string("");
  };
};

template <typename T>
T from_string(const std::string &str) {
  return T();
};

template <>
Type from_string<Type>(const std::string &action_str) {
  switch (action_str.length()) {
    case 3:
      return Type::msg;
    case 4:
      return Type::movement;
    case 7:
      return (action_str[0] == 'T') ? Type::torpedo : Type::surface;
    default:
      return Type::msg;
  };
};

template <>
move::Power from_string<move::Power>(const std::string &powa_str) {
  switch (powa_str.length()) {
    case 7:
      return move::Power::torpedo;
    default:
      return move::Power::none;
  };
};

}  // namespace str

}  // namespace action

void updateEnemyLocationFromMove(const ocean::map_t &ocean,
                                 const action::move::Direction &enemy_dir,
                                 ocean::path_list_t &enemy_possible_path_list) {
  ocean::Position position_increment = {.x = 0, .y = 0};
  switch (enemy_dir) {
    case action::move::Direction::North:
      position_increment.y = -1;
      break;
    case action::move::Direction::South:
      position_increment.y = 1;
      break;
    case action::move::Direction::East:
      position_increment.x = 1;
      break;
    case action::move::Direction::West:
      position_increment.x = -1;
      break;
  };
  for (auto enemy_possible_path = enemy_possible_path_list.begin();
       enemy_possible_path != enemy_possible_path_list.end();) {
    // Retreive last position
    // Update it with the movement orders
    ocean::Position new_position{
      .x = enemy_possible_path->back().x + position_increment.x,
      .y = enemy_possible_path->back().y + position_increment.y
    };

    // Check map boundaries and isles
    if (new_position.y < 0 || new_position.y >= ocean.size() ||
        new_position.x < 0 || new_position.x >= ocean[new_position.y].size() ||
        ocean[new_position.y][new_position.x] & OCEAN_TILE_ISLE) {
      // either out of bound OR isles -> delete the path
      enemy_possible_path = enemy_possible_path_list.erase(enemy_possible_path);
    } else {
      // Still in water -> add new position to path
      enemy_possible_path->push_back(std::move(new_position));
      ++enemy_possible_path;
    }
  };
};

}  // namespace submarine

inline std::pair<ocean::map_t, player_id_t> initialize() {
  std::cerr << "-- Initializing --" << std::endl;
  int width;
  int height;
  player_id_t my_id;
  ocean::map_t ocean;

  std::cin >> width >> height >> my_id;
  std::cin.ignore();

  std::cerr << "Initial game data retreived: height=" << height
            << ", width=" << width << ", my_id=" << my_id << std::endl;

  ocean.reserve(height);

  for (int i = 0; i < height; i++) {
    std::string line_str;
    ocean::line_t horizontal_ocean_line;

    getline(std::cin, line_str);
    std::cerr << "Ocean Line " << i << " received: " << line_str << std::endl;

    horizontal_ocean_line.reserve(line_str.size());
    for (const auto &init_tile : line_str)
      horizontal_ocean_line.emplace_back((init_tile == 'x') ? OCEAN_TILE_ISLE
                                                            : OCEAN_TILE_FREE);

    std::cerr << "Adding new horizontal line of "
              << horizontal_ocean_line.size() << " tiles" << std::endl;

    ocean.push_back(std::move(horizontal_ocean_line));
  }

  return std::make_pair(ocean, my_id);
}

int main() {
  player_id_t my_id;
  ocean::map_t ocean;
  submarine::Player me;
  submarine::Player enemy;
  ocean::path_list_t enemy_possible_path_list;

  std::tie(ocean, my_id) = initialize();

  for (std::size_t y = 0; y < ocean.size(); ++y)
    for (std::size_t x = 0; x < ocean[y].size(); ++x)
      if (ocean[y][x] & OCEAN_TILE_FREE)
        enemy_possible_path_list.emplace_back(1,
                                              ocean::Position{.x = x, .y = y});

  std::cerr << enemy_possible_path_list.size() << " Possible enemy paths"
            << std::endl;

  // Write an action using std::cout. DON'T FORGET THE "<< std::endl"
  // To debug: cerr << "Debug messages..." << std::endl;
  me.pos.x = me.pos.y = 0;

  // Init pos
  {
    bool found = false;
    for (const auto &line : ocean) {
      for (const auto &cell : line)
        if ((cell & OCEAN_TILE_MASK) == OCEAN_TILE_FREE) {
          found = true;
          break;
        } else
          ++me.pos.x;

      if (found) break;
      ++me.pos.y;
    }
  }

  std::cout << me.pos.x << " " << me.pos.x << std::endl;

  // game loop
  while (1) {
    std::cin >> me.pos.x >> me.pos.y >> me.hp >> enemy.hp >> me.torpedo_cd >>
        me.sonar_cd >> me.silence_cd >> me.mine_cd;
    std::cin.ignore();

    std::string sonarResult;
    std::cin >> sonarResult;
    std::cin.ignore();
    std::cerr << "sonar = " << sonarResult << std::endl;

    std::string opponentOrders;
    getline(std::cin, opponentOrders);
    std::cerr << "Opp = " << opponentOrders << std::endl;

    // FIXME: THIS IS TMP, JUST FOR TESTS
    auto move_idx = opponentOrders.find("MOVE");
    if (move_idx != std::string::npos) {
      // Opp moved
      submarine::updateEnemyLocationFromMove(
          ocean,
          static_cast<submarine::action::move::Direction>(
              opponentOrders[move_idx + 5]),
          enemy_possible_path_list);

      std::cerr << enemy_possible_path_list.size() << " Possible enemy path"
                << std::endl;
    }

    // FIXME: THIS IS TMP, JUST FOR TESTS

    // Write an action using std::cout. DON'T FORGET THE "<< std::endl"
    // To debug: cerr << "Debug messages..." << std::endl;
    ocean[me.pos.y][me.pos.x] |= OCEAN_TILE_VISITED;

    for (const auto &possible_path : enemy_possible_path_list)
      ocean[possible_path.back().y][possible_path.back().x] |=
          OCEAN_TILE_TARGET;

    std::cerr << ocean::draw(ocean) << std::endl;

    for (const auto &possible_path : enemy_possible_path_list)
      ocean[possible_path.back().y][possible_path.back().x] &=
          ~OCEAN_TILE_TARGET;

    std::cout << "MOVE S TORPEDO" << std::endl;
  }
}
