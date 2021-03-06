
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

// This is for my A*
#include <deque>
#include <functional>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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

namespace path {

template <class PositionCoordinatesType,
          class Hash = std::hash<PositionCoordinatesType>,
          class PositionEqual = std::equal_to<PositionCoordinatesType>>
std::deque<PositionCoordinatesType> aStarShortestPath(
    const PositionCoordinatesType &from_position,
    const PositionCoordinatesType &to_position,
    std::function<double(const PositionCoordinatesType &)> computeHeuristic,
    std::function<std::vector<std::pair<PositionCoordinatesType, double>>(
        const PositionCoordinatesType &)>
        getNeighborWeightedPositions) {
  auto position_are_equals = PositionEqual();

  typedef std::pair<PositionCoordinatesType, double> node_t;

  auto compare_f_score = [](const node_t &lhs, const node_t &rhs) {
    return lhs.second > rhs.second;
  };

  typedef std::priority_queue<node_t, std::vector<node_t>,
                              decltype(compare_f_score)>
      node_heap_t;

  typedef std::unordered_map<PositionCoordinatesType, double, Hash,
                             PositionEqual>
      score_map_t;

  typedef std::unordered_map<PositionCoordinatesType, PositionCoordinatesType,
                             Hash, PositionEqual>
      path_map_t;

  typedef std::unordered_set<PositionCoordinatesType, Hash, PositionEqual>
      visited_pos_set_t;

  std::deque<PositionCoordinatesType> output_path;
  node_heap_t node_heap(compare_f_score);
  visited_pos_set_t visited_position(0);
  score_map_t g_score_map;
  path_map_t came_from;

  node_heap.emplace(from_position, computeHeuristic(from_position));
  g_score_map.emplace(from_position, 0);

  while (!node_heap.empty()) {
    PositionCoordinatesType current_position;
    double current_f_score;

    std::tie(current_position, current_f_score) = node_heap.top();
    node_heap.pop();

    visited_position.insert(current_position);

    if (position_are_equals(current_position, to_position)) {
      // Found -> reconstruct path
      output_path.push_back(current_position);
      auto pos_it = came_from.find(current_position);
      while (pos_it != came_from.end()) {
        output_path.push_back(pos_it->second);
        pos_it = came_from.find(pos_it->second);
      }

      break;

    } else {
      // explore
      for (const auto &neighbour_info :
           getNeighborWeightedPositions(current_position)) {
        PositionCoordinatesType neighbour_position;
        double neighbour_distance;

        std::tie(neighbour_position, neighbour_distance) = neighbour_info;

        // Compute the possible new score from this node to the neighbor
        double new_g_score = g_score_map[current_position] + neighbour_distance;

        auto neighbour_g_score_it = g_score_map.find(neighbour_position);

        // Get the neighbour g_score
        if (neighbour_g_score_it == g_score_map.end())
          std::tie(neighbour_g_score_it, std::ignore) = g_score_map.insert(
              {neighbour_position, std::numeric_limits<double>::infinity()});

        if (new_g_score < neighbour_g_score_it->second) {
          // Better g_score than neighbour
          came_from[neighbour_position] = current_position;
          g_score_map[neighbour_position] = new_g_score;

          if (visited_position.find(neighbour_position) ==
              visited_position.end()) {
            node_heap.emplace(
                neighbour_position,
                new_g_score + computeHeuristic(neighbour_position));
          }
        }
      }
    }
  }

  return output_path;
}

template <class PositionCoordinatesType,
          class Hash = std::hash<PositionCoordinatesType>,
          class PositionEqual = std::equal_to<PositionCoordinatesType>>
std::deque<PositionCoordinatesType> aStarShortestPath(
    const PositionCoordinatesType &from_position,
    const PositionCoordinatesType &to_position,
    std::function<double(const PositionCoordinatesType &)> computeHeuristic,
    std::function<
        std::vector<PositionCoordinatesType>(const PositionCoordinatesType &)>
        getNeighborPositions) {
  auto position_are_equals = PositionEqual();

  typedef std::pair<PositionCoordinatesType, double> node_t;

  auto compare_f_score = [](const node_t &lhs, const node_t &rhs) {
    return lhs.second > rhs.second;
  };

  typedef std::priority_queue<node_t, std::vector<node_t>,
                              decltype(compare_f_score)>
      node_heap_t;

  typedef std::unordered_map<PositionCoordinatesType, double, Hash,
                             PositionEqual>
      score_map_t;

  typedef std::unordered_map<PositionCoordinatesType, PositionCoordinatesType,
                             Hash, PositionEqual>
      path_map_t;

  typedef std::unordered_set<PositionCoordinatesType, Hash, PositionEqual>
      visited_pos_set_t;

  std::deque<PositionCoordinatesType> output_path;
  node_heap_t node_heap(compare_f_score);
  visited_pos_set_t visited_position(0);
  score_map_t g_score_map;
  path_map_t came_from;

  node_heap.emplace(from_position, computeHeuristic(from_position));
  g_score_map.emplace(from_position, 0);

  while (!node_heap.empty()) {
    PositionCoordinatesType current_position;
    double current_f_score;

    std::tie(current_position, current_f_score) = node_heap.top();
    node_heap.pop();

    visited_position.insert(current_position);

    if (position_are_equals(current_position, to_position)) {
      // Found -> reconstruct path
      output_path.push_back(current_position);
      auto pos_it = came_from.find(current_position);
      while (pos_it != came_from.end()) {
        output_path.push_back(pos_it->second);
        pos_it = came_from.find(pos_it->second);
      }

      break;

    } else {
      // explore
      for (const auto &neighbour_position :
           getNeighborPositions(current_position)) {
        // Compute the possible new score from this node to the neighbor
        double new_g_score = g_score_map[current_position] + 1;

        auto neighbour_g_score_it = g_score_map.find(neighbour_position);

        // Get the neighbour g_score
        if (neighbour_g_score_it == g_score_map.end())
          std::tie(neighbour_g_score_it, std::ignore) = g_score_map.insert(
              {neighbour_position, std::numeric_limits<double>::infinity()});

        if (new_g_score < neighbour_g_score_it->second) {
          // Better g_score than neighbour
          came_from[neighbour_position] = current_position;
          g_score_map[neighbour_position] = new_g_score;

          if (visited_position.find(neighbour_position) ==
              visited_position.end()) {
            node_heap.emplace(
                neighbour_position,
                new_g_score + computeHeuristic(neighbour_position));
          }
        }
      }
    }
  }

  return output_path;
}

}  // namespace path

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
  if (cell & OCEAN_TILE_ISLE) return std::string("X");
  if (cell & OCEAN_TILE_TARGET) return std::string("T");
  if (cell & OCEAN_TILE_VISITED) return std::string("O");
  return std::string(" ");
};

typedef std::vector<cell_t> line_t;

typedef std::vector<line_t> map_t;

#define OCEAN_SECTOR_SIZE_X 5
#define OCEAN_SECTOR_SIZE_Y 5
typedef std::uint32_t sector_t;

struct Position {
  int x, y;

  bool operator==(const Position &other) const {
    return (x == other.x) && (y == other.y);
  }

  char go_to(const Position &target) const {
    if (x == target.x) {
      if (y + 1 == target.y)
        return 'S';
      else if (y - 1 == target.y)
        return 'N';
      else
        return 'S';  // <-- IMPOSSIBLE
    } else {
      if (x + 1 == target.x)
        return 'E';
      else if (x - 1 == target.x)
        return 'W';
      else
        return 'W';  // <-- IMPOSSIBLE
    }
  };
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

std::ostream &operator<<(std::ostream &output, const std::pair<int, int> &pos) {
  output << pos.first << " " << pos.second;
  return output;
};

// std::ostream &operator<<(std::ostream &output,
//                          const ocean::action::action_t &actions) {
//   for (const auto &action : actions) output << action << " |";

//   return output;
// };

namespace std {
template <>
struct hash<ocean::Position> {
  std::size_t operator()(const ocean::Position &pos) const {
    std::size_t linear_coord = pos.x + pos.y * (15);
    return std::hash<std::size_t>()(linear_coord);
  };
};
}  // namespace std

///////////////////////////////////////////////////////////////////////////////
//                                 FUNCTIONS                                 //
///////////////////////////////////////////////////////////////////////////////

namespace ocean {

// OCEAN STR FORMAT ///////////////////////////////////////////////////////////

void removeMask(map_t &map, const cell_t &mask) {
  for (auto &line : map)
    for (auto &cell : line) cell &= ~mask;
};

constexpr double computeL1Distance(const Position &position_1,
                                   const Position &position_2) {
  return abs((position_1.x - position_2.x) + (position_1.y - position_2.y));
};

constexpr double computeL2Distance(const Position &position_1,
                                   const Position &position_2) {
  return sqrt(pow((position_1.x - position_2.x), 2) +
              pow((position_1.y - position_2.y), 2));
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

constexpr bool isPositionOutsideOcean(const Position &position,
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

Position getNewPositionFromMove(const char &move, const Position &position) {
  Position new_pos = position;
  switch (move) {
    case 'N':
      new_pos.y -= 1;
      break;
    case 'S':
      new_pos.y += 1;
      break;
    case 'E':
      new_pos.x += 1;
      break;
    case 'W':
      new_pos.x -= 1;
      break;
  };
  return new_pos;
};

std::vector<ocean::Position> validNextPositionListAround(
    const Position &position, const map_t &ocean, const cell_t &invalid_cell) {
  std::vector<ocean::Position> out;
  out.reserve(4);

  std::array<char, 4> move_dirs = {'N', 'S', 'E', 'W'};
  for (const auto &move : move_dirs) {
    auto new_pos = ocean::action::getNewPositionFromMove(move, position);
    if (!ocean::isPositionOutsideOcean(new_pos, ocean) &&
        (ocean[new_pos.y][new_pos.x] & invalid_cell) == 0) {
      out.push_back(new_pos);
    }
  }

  out.shrink_to_fit();
  return out;
};

std::vector<ocean::Position> validNextPositionListAround(
    const Position &position, const map_t &ocean) {
  return validNextPositionListAround(position, ocean, OCEAN_TILE_ISLE);
};

void updatePositionPredictionFromMove(path_list_t &path_prediction_list,
                                      const map_t &ocean, const move_t &move) {
  std::cerr << "Update position pred from move dir " << move.first << std::endl;
  Position position_increment = {.x = 0, .y = 0};
  updatePositionFromMove(move.first, position_increment);

  for (auto possible_path = path_prediction_list.begin();
       possible_path != path_prediction_list.end();) {
    // Retreive last position
    // Update it with the movement orders
    Position new_position{.x = possible_path->back().x + position_increment.x,
                          .y = possible_path->back().y + position_increment.y};

    // Check map boundaries and isles
    if (isPositionOutsideOcean(new_position, ocean) ||
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
                              const map_t &ocean,
                              const std::string &enemy_orders) {
  std::stringstream enemy_orders_stream(enemy_orders);
  std::string single_order;

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
          updatePositionPredictionFromMove(path_prediction_list, ocean,
                                           parse::moveStr(single_order_stream));
        }
        break;

      case 'T':
        updatePositionPredictionFromTorpedo(
            path_prediction_list, ocean,
            parse::torpedoStr(single_order_stream));
        break;

      case 'S':
        if (action_word[1] == 'U')
          updatePositionPredictionFromSurface(
              path_prediction_list, ocean,
              parse::surfaceStr(single_order_stream));
        break;
    };
  }
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
        free_pos.emplace_back(
            1, ocean::Position{.x = horizontal_ocean_line.size(), .y = i});
        horizontal_ocean_line.emplace_back(OCEAN_TILE_FREE);
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
  std::deque<ocean::Position> current_path;
  ocean::Position target{.x = 7, .y = 7};

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
      ocean::action::updatePositionPrediction(enemy_prediction_path_list, ocean,
                                              enemy_orders);

    ocean[me.pos.y][me.pos.x] |= OCEAN_TILE_VISITED;

    if (current_path.empty()) {
      std::cerr << "Compute new path" << std::endl;
      // We have no cap !
      current_path = std::move(utils::path::aStarShortestPath<ocean::Position>(
          me.pos, target,
          std::bind(ocean::computeL1Distance, std::placeholders::_1, target),
          std::bind<std::vector<ocean::Position>(const ocean::Position &,
                                                 const ocean::map_t &,
                                                 const ocean::cell_t &)>(
              ocean::action::validNextPositionListAround, std::placeholders::_1,
              ocean, (OCEAN_TILE_ISLE | OCEAN_TILE_VISITED))));
      current_path.pop_back();
      std::cerr << "New path size: " << current_path.size() << std::endl;
    }

    std::cerr << ocean << std::endl;
    std::cerr << "Me: " << me.pos << std::endl;
    std::cerr << "Next: " << current_path.back() << std::endl;
    std::cout << "MOVE " << me.pos.go_to(current_path.back()) << " TORPEDO"
              << std::endl;
    current_path.pop_back();
  }
}
