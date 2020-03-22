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
#define OCEAN_TILE_MASK 0b000011
#define OCEAN_TILE_FREE 0b000000
#define OCEAN_TILE_ISLE 0b000001
#define OCEAN_TILE_VISITED 0b000010

#define OCEAN_N_TILE_POSI 2
#define OCEAN_N_TILE_MASK 0b000100

#define OCEAN_S_TILE_POSI 3
#define OCEAN_S_TILE_MASK 0b001000

#define OCEAN_E_TILE_POSI 4
#define OCEAN_E_TILE_MASK 0b010000

#define OCEAN_W_TILE_POSI 5
#define OCEAN_W_TILE_MASK 0b100000

typedef std::uint32_t cell_t;
typedef std::vector<cell_t> line_t;
typedef std::vector<line_t> map_t;

// OCEAN STR FORMAT ///////////////////////////////////////////////////////////
inline const std::string draw(const cell_t &cell) noexcept {
  switch (cell & OCEAN_TILE_MASK) {
    case OCEAN_TILE_FREE:
      // if (cell & OCEAN_W_TILE_MASK) return std::string("W");
      // else if (cell & OCEAN_E_TILE_MASK) return std::string("E");
      // else if (cell & OCEAN_N_TILE_MASK) return std::string("N");
      // else if (cell & OCEAN_S_TILE_MASK) return std::string("S");
      // else return std::string(" ");
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
    for (const auto &init_tile : line_str) {
      // Add 1 Tile based on the character
      ocean::cell_t new_cell((init_tile == 'x') ? OCEAN_TILE_ISLE
                                                : OCEAN_TILE_FREE);

      // FIXME: Lot of if, ... I'm not proud maybe make it more beautiful ?
      if (ocean.size() == 0)  // First line -> Mark North as occupied
        new_cell |= OCEAN_N_TILE_MASK;
      else if (ocean.size() == (height - 1))  // Last line -> South occupied
        new_cell |= OCEAN_S_TILE_MASK;
      else  // Middle
      {
        // We check and update the neighbor above
        ocean::cell_t &neigh_above = ocean.back()[horizontal_ocean_line.size()];

        // Update the cell above with cell below
        neigh_above |= ((new_cell & OCEAN_TILE_ISLE) << OCEAN_S_TILE_POSI);

        // Update the new cell with above infos
        new_cell |= (neigh_above & OCEAN_TILE_ISLE) << OCEAN_N_TILE_POSI;
      }

      if (horizontal_ocean_line.size() == 0)  // First cell of row -> West Occ
        new_cell |= OCEAN_W_TILE_MASK;
      else  // We check and update the neighbor on the left
      {
        ocean::cell_t &neigh_left = horizontal_ocean_line.back();

        // Update the cell on the left with right cell infos
        neigh_left |= ((new_cell & OCEAN_TILE_ISLE) << OCEAN_E_TILE_POSI);

        // Update the new cell with left info
        new_cell |= (neigh_left & OCEAN_TILE_ISLE) << OCEAN_W_TILE_POSI;
      }

      horizontal_ocean_line.push_back(std::move(new_cell));
    }

    // Last element of single row: East Occupied
    horizontal_ocean_line.back() |= OCEAN_E_TILE_MASK;

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

    // Write an action using std::cout. DON'T FORGET THE "<< std::endl"
    // To debug: cerr << "Debug messages..." << std::endl;
    ocean[me.pos.y][me.pos.x] |= OCEAN_TILE_VISITED;
    std::cerr << ocean::draw(ocean) << std::endl;

    std::cout << "MOVE S TORPEDO" << std::endl;
  }
}
