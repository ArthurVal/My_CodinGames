#include <cstdlib>  // size_t

#include <cstdint>
#include <iostream>
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
//                                SUBMARINE                                  //
///////////////////////////////////////////////////////////////////////////////
namespace submarine {

// PLAYER TYPES ///////////////////////////////////////////////////////////////
struct Position {
  std::size_t x, y;
};

struct Player {
  Position pos;
  int hp;
  int torpedo_cd;
  int sonar_cd;
  int silence_cd;
  int mine_cd;
};
}  // namespace submarine

///////////////////////////////////////////////////////////////////////////////
//                                   OCEAN                                   //
///////////////////////////////////////////////////////////////////////////////
namespace ocean {

// OCEAN TYPES ////////////////////////////////////////////////////////////////
#define OCEAN_TILE_FREE 0b00
#define OCEAN_TILE_ISLE 0b01
#define OCEAN_TILE_VISITED 0b10

typedef std::uint32_t cell_t;
typedef std::vector<cell_t> line_t;
typedef std::vector<line_t> map_t;

// OCEAN STR FORMAT ///////////////////////////////////////////////////////////
inline const std::string draw(const cell_t &cell) noexcept {

  switch (cell) {
    case OCEAN_TILE_FREE:
      return std::string(" ");
    case OCEAN_TILE_VISITED:
      return std::string("O");
    case OCEAN_TILE_ISLE:
      return std::string("X");
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
void clean(map_t &map) {
  for (auto &line : map)
    for (auto &cell : line) cell &= ~OCEAN_TILE_VISITED;
}

}  // namespace ocean

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
      horizontal_ocean_line.push_back((init_tile == '.') ? OCEAN_TILE_FREE
                                                         : OCEAN_TILE_ISLE);

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

  std::tie(ocean, my_id) = initialize();

  // Write an action using std::cout. DON'T FORGET THE "<< std::endl"
  // To debug: cerr << "Debug messages..." << std::endl;

  std::cout << "7 7" << std::endl;

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

    // Write an action using std::cout. DON'T FORGET THE "<< std::endl"
    // To debug: cerr << "Debug messages..." << std::endl;
    ocean[me.pos.y][me.pos.x] |= OCEAN_TILE_VISITED;
    std::cerr << ocean::draw(ocean) << std::endl;

    std::cout << "MOVE N TORPEDO" << std::endl;
  }
}
