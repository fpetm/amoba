#include "amoba.hpp"
#include <amoba/amoba.h>
#include <amoba/amobas/inf_am.h>

#include <cstdint>
#include <vector>

constexpr uint8_t g_Dimension = 3;
constexpr uint64_t g_Win = 3, g_ChunkSize = 32;

int main() {
  am_amoba amoba = am_amoba_list[0];
  am_inf_am_parameters params = {g_Win, g_ChunkSize, g_Dimension};
  amoba.init_fun(&amoba, &params);
  while (true) {
    std::cout << "P> ";
    std::vector<int64_t> position(g_Dimension);
    for (auto &p : position)
      std::cin >> p;
    color_t color;
    std::cout << "C> ";
    std::cin >> color;

    am_inf_am_position ampos;
    ampos.dimension = g_Dimension;
    for (uint8_t i = 0; i < g_Dimension; i++) {
      ampos.component[i] = position[i];
    }

    amoba.set_fun(&amoba, color, &ampos);

    color_t win = amoba.check_fun(&amoba, &ampos);
    if (win) {
      std::cout << win << " has won! Goodbye!" << std::endl;
      return 0;
    } else {
      std::cout << "--" << std::endl;
    }
  }
  return 0;
}
