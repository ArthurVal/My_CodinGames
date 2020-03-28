
#include <cmath>
#include <cstdint>
#include <iostream>
#include <list>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>    // std::tie
#include <utility>  // pair
#include <vector>

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

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
//                                   TYPES                                   //
///////////////////////////////////////////////////////////////////////////////

typedef int player_id_t;

namespace ocean {
// OCEAN TYPES ////////////////////////////////////////////////////////////////

// -- OCEAN map tiles values
#define OCEAN_TILE_MASK 0b0000011
#define OCEAN_TILE_FREE 0b0000001
#define OCEAN_TILE_ISLE 0b0000010

#define OCEAN_TILE_PLAYER_MASK 0b0001100
#define OCEAN_TILE_VISITED 0b0000100
#define OCEAN_TILE_TARGET 0b0001000

typedef std::uint32_t cell_t;
std::string draw(const cell_t &cell) {
  switch (cell) {
    case OCEAN_TILE_FREE:
      return std::string(" ");
      break;
    case OCEAN_TILE_FREE | OCEAN_TILE_VISITED:
      return std::string("O");
      break;
    case OCEAN_TILE_ISLE:
      return std::string("X");
      break;
    case OCEAN_TILE_FREE | OCEAN_TILE_TARGET:
      return std::string("T");
      break;
    default:
      return std::string("?");
      break;
  }
};

typedef std::vector<cell_t> line_t;

typedef std::vector<line_t> map_t;

#define OCEAN_SECTOR_SIZE_X 5
#define OCEAN_SECTOR_SIZE_Y 5
typedef std::uint32_t sector_t;

struct Position {
  int x, y;
};

typedef std::vector<Position> path_t;
typedef std::list<path_t> path_list_t;

struct Submarine {
  Position pos;
  int hp;
  int torpedo_cd;
  int sonar_cd;
  int silence_cd;
  int mine_cd;
};

namespace action {

// The types of actions
enum ActionType { move, torpedo, surface, msg };

// Types associated to MOVE actions
enum MovePower { reload_torpedo, reload_silence, reload_sonar, reload_mine };
typedef std::pair<char, std::optional<MovePower>> move_t;

namespace parse {

std::optional<MovePower> movePowerStr(std::stringstream &move_sstream) {
  std::string power_string;
  move_sstream >> power_string;
  switch (power_string.front()) {
    case 'T':
      return std::make_optional(MovePower::reload_torpedo);
    default:
      return {};
  };
};

move_t moveStr(std::stringstream &move_sstream) {
  char move_dir;
  move_sstream >> move_dir;
  return std::make_pair(move_dir, movePowerStr(move_sstream));
};

}  // namespace parse

// Types associated to TORPEDO actions
#define OCEAN_TORPEDO_SHOT_RANGE 4
#define OCEAN_TORPEDO_SHOT_RANGE_2 \
  OCEAN_TORPEDO_SHOT_RANGE *OCEAN_TORPEDO_SHOT_RANGE

namespace parse {
Position torpedoStr(std::stringstream &torpedo_sstream) {
  Position torpedo_target;
  torpedo_sstream >> torpedo_target.x >> torpedo_target.y;
  return torpedo_target;
};
}  // namespace parse

namespace parse {
sector_t surfaceStr(std::stringstream &surface_sstream) {
  sector_t sector_number;
  surface_sstream >> sector_number;
  return sector_number;
};
}  // namespace parse

typedef std::tuple<std::optional<move_t>,    // Move
                   std::optional<Position>,  // Torpedo
                   std::optional<sector_t>   // Torpedo
                   >
    action_t;

namespace parse {

action_t enemyActions(std::string enemy_orders) {
  std::stringstream enemy_orders_stream(enemy_orders);
  std::string single_order;

  std::optional<move_t> move_action;
  std::optional<Position> torpedo_action;
  std::optional<sector_t> surface_action;

  std::cerr << "Parsing orders: " << enemy_orders_stream.str() << std::endl;

  while (getline(enemy_orders_stream, single_order, '|')) {
    std::stringstream single_order_stream(single_order);
    std::string action_word;

    std::cerr << "New order: " << single_order << std::endl;

    single_order_stream >> action_word;
    std::cerr << "Action: " << action_word << std::endl;

    switch (action_word.front()) {
      case 'M':
        if (action_word[1] == 'O') {
          std::cerr << "Adding new movement" << std::endl;
          move_action = moveStr(single_order_stream);
        }
        break;

      case 'T':
        std::cerr << "Adding new torpedo" << std::endl;
        torpedo_action = torpedoStr(single_order_stream);
        break;

      case 'S':
        std::cerr << "Adding new surface" << std::endl;
        surface_action = surfaceStr(single_order_stream);
        break;
    };
  }
  return std::make_tuple(move_action, torpedo_action, surface_action);
}

}  // namespace parse

}  // namespace action

}  // namespace ocean

///////////////////////////////////////////////////////////////////////////////
//                              OSTREAM OVERLOAD                             //
///////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream &output, const ocean::line_t &line) {
  for (const auto &cell : line) output << ocean::draw(cell);
  return output;
};

std::ostream &operator<<(std::ostream &output, const ocean::map_t &map) {
  output << "Current Map (" << map.size() << "x" << map[0].size() << "):\n";
  output << "*" << utils::ascii::horizontalBar(map.size()) << "*\n";
  for (const auto &horizontal_line : map)
    output << "|" << horizontal_line << "|\n";

  output << "*" << utils::ascii::horizontalBar(map.size()) << "*\n";

  return output;
};

std::ostream &operator<<(std::ostream &output, const ocean::Position &pos) {
  output << pos.x << " " << pos.y;
  return output;
};

// std::ostream &operator<<(std::ostream &output,
//                          const ocean::action::action_t &actions) {
//   for (const auto &action : actions) output << action << " |";

//   return output;
// };

///////////////////////////////////////////////////////////////////////////////
//                                 FUNCTIONS                                 //
///////////////////////////////////////////////////////////////////////////////

namespace ocean {

// OCEAN STR FORMAT ///////////////////////////////////////////////////////////

void removeMask(map_t &map, const cell_t &mask) {
  for (auto &line : map)
    for (auto &cell : line) cell &= ~mask;
};

constexpr bool isXInsideSector(const int &x, const sector_t &sector) {
  return (x >= (sector - 1) * OCEAN_SECTOR_SIZE_X) &&
         (x < (sector - 1) * OCEAN_SECTOR_SIZE_X + OCEAN_SECTOR_SIZE_X);
};

constexpr bool isYInsideSector(const int &y, const sector_t &sector) {
  return (y >= (sector - 1) * OCEAN_SECTOR_SIZE_Y) &&
         (y < (sector - 1) * OCEAN_SECTOR_SIZE_Y + OCEAN_SECTOR_SIZE_Y);
};

constexpr bool isPositionInsideSector(const Position &position,
                                      const sector_t &sector) {
  return isXInsideSector(position.x, sector) &&
         isYInsideSector(position.y, sector);
};

constexpr bool isPositionInsideOcean(const Position &position,
                                     const map_t &ocean) {
  return (position.y < 0 || position.y >= ocean.size() || position.x < 0 ||
          position.x >= ocean[position.y].size());
};

constexpr bool isPositionInTorpedoRange(const Position &position,
                                        const Position &torpedo_target) {
  return (pow(torpedo_target.x - position.x, 2) +
          pow(torpedo_target.y - position.y, 2)) <= OCEAN_TORPEDO_SHOT_RANGE_2;
};

namespace action {

void updatePositionFromMove(const char &move, Position &position) {
  switch (move) {
    case 'N':
      position.y -= 1;
      break;
    case 'S':
      position.y += 1;
      break;
    case 'E':
      position.x += 1;
      break;
    case 'W':
      position.x -= 1;
      break;
  };
};

void updatePositionPredictionFromMove(path_list_t &path_prediction_list,
                                      const map_t &ocean, const move_t &move) {
  std::cerr << "Update position pred from move dir " << move.first
            << std::endl;
  Position position_increment = {.x = 0, .y = 0};
  updatePositionFromMove(move.first, position_increment);

  for (auto possible_path = path_prediction_list.begin();
       possible_path != path_prediction_list.end();) {
    // Retreive last position
    // Update it with the movement orders
    Position new_position{.x = possible_path->back().x + position_increment.x,
                          .y = possible_path->back().y + position_increment.y};

    // Check map boundaries and isles
    if (isPositionInsideOcean(new_position, ocean) ||
        ocean[new_position.y][new_position.x] & OCEAN_TILE_ISLE) {
      // either out of bound OR isles -> delete the path
      possible_path = path_prediction_list.erase(possible_path);
    } else {
      // Still in water -> add new position to path
      possible_path->push_back(std::move(new_position));
      ++possible_path;
    }
  };
};

void updatePositionPredictionFromSurface(path_list_t &path_prediction_list,
                                         const map_t &ocean,
                                         const sector_t &surface_sector) {
  std::cerr << "Update position pred from surface sector " << surface_sector
            << std::endl;
  for (auto possible_path = path_prediction_list.begin();
       possible_path != path_prediction_list.end();) {
    // Retreive last position
    // Update it with the movement orders

    // Check map boundaries and isles
    if (!isPositionInsideSector(possible_path->back(), surface_sector)) {
      possible_path = path_prediction_list.erase(possible_path);
    } else {
      ++possible_path;
    }
  };
};

void updatePositionPredictionFromTorpedo(path_list_t &path_prediction_list,
                                         const map_t &ocean,
                                         const Position &torpedo_target) {
  std::cerr << "Update position pred from torpedo target " << torpedo_target
            << std::endl;
  for (auto possible_path = path_prediction_list.begin();
       possible_path != path_prediction_list.end();) {
    // Retreive last position
    // Update it with the movement orders

    // Check map boundaries and isles
    if (!isPositionInTorpedoRange(possible_path->back(), torpedo_target)) {
      possible_path = path_prediction_list.erase(possible_path);
    } else {
      ++possible_path;
    }
  };
};

void updatePositionPrediction(path_list_t &path_prediction_list,
                              const map_t &ocean, const action_t &actions) {
  std::optional<move_t> move_action;
  std::optional<Position> torpedo_action;
  std::optional<sector_t> surface_action;

  std::tie(move_action, torpedo_action, surface_action) = actions;

  if (move_action)
    updatePositionPredictionFromMove(path_prediction_list, ocean,
                                     move_action.value());

  if (torpedo_action)
    updatePositionPredictionFromTorpedo(path_prediction_list, ocean,
                                        torpedo_action.value());

  if (surface_action)
    updatePositionPredictionFromSurface(path_prediction_list, ocean,
                                        surface_action.value());
}

}  // namespace action

}  // namespace ocean

inline std::tuple<ocean::map_t, ocean::path_list_t, player_id_t> initialize() {
  std::cerr << "-- Initializing --" << std::endl;
  int width, height;
  player_id_t my_id;
  ocean::map_t ocean;
  ocean::path_list_t free_pos;

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
      if (init_tile == 'x') {
        horizontal_ocean_line.emplace_back(OCEAN_TILE_ISLE);
      } else {
        horizontal_ocean_line.emplace_back(OCEAN_TILE_FREE);
        free_pos.emplace_back(
            1, ocean::Position{.x = horizontal_ocean_line.size(), .y = i});
      }

    std::cerr << "Adding new horizontal line of "
              << horizontal_ocean_line.size() << " tiles" << std::endl;

    ocean.push_back(std::move(horizontal_ocean_line));
  }
  std::cerr << ocean << std::endl;
  return std::make_tuple(ocean, free_pos, my_id);
}

int main() {
  player_id_t my_id;
  ocean::map_t ocean;
  ocean::Submarine me, enemy;
  ocean::path_list_t my_prediction_path_list, enemy_prediction_path_list;

  std::tie(ocean, enemy_prediction_path_list, my_id) = initialize();

  my_prediction_path_list = enemy_prediction_path_list;

  // for (std::size_t y = 0; y < ocean.size(); ++y)
  //   for (std::size_t x = 0; x < ocean[y].size(); ++x)
  //     if (ocean[y][x] & OCEAN_TILE_FREE)
  //       enemy_possible_path_list.emplace_back(1,
  //                                             ocean::Position{.x = x, .y =
  //                                             y});

  std::cerr << enemy_prediction_path_list.size() << " Possible enemy paths"
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

    std::string enemy_orders;
    getline(std::cin, enemy_orders);
    if (enemy_orders != "NA")
      ocean::action::updatePositionPrediction(
          enemy_prediction_path_list, ocean,
          ocean::action::parse::enemyActions(enemy_orders));

    // FIXME: THIS IS TMP, JUST FOR TESTS

    // Write an action using std::cout. DON'T FORGET THE "<< std::endl"
    // To debug: cerr << "Debug messages..." << std::endl;
    ocean[me.pos.y][me.pos.x] |= OCEAN_TILE_VISITED;

    for (const auto &possible_path : enemy_prediction_path_list)
      ocean[possible_path.back().y][possible_path.back().x] |=
          OCEAN_TILE_TARGET;

    std::cerr << ocean << std::endl;

    for (const auto &possible_path : enemy_prediction_path_list)
      ocean[possible_path.back().y][possible_path.back().x] &=
          ~OCEAN_TILE_TARGET;

    std::cout << "MOVE S TORPEDO" << std::endl;
  }
}
