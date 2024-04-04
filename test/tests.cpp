#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>
#include <amoba/amoba.h>
#include <amoba/amobas/inf_am.h>

#include <iostream>

am_inf_am_position position(uint8_t dimension) {
  am_inf_am_position p;
    p.dimension = uint8_t(dimension);
    p.component[0] = 0;
    p.component[1] = 0;
    p.component[2] = 0;
    p.component[3] = 0;
    return p;
}

TEST_CASE( "Infinite amoba board", "[inf_am]") {
  auto chunk_size = GENERATE(8, 16, 32);
//  auto chunk_size = GENERATE(2, 4, 8, 128, 1024/*,32768*/);
  auto dimension = GENERATE(2, 3);
  auto win = GENERATE(3, 5, 10);

  am_amoba amoba = am_amoba_list[0];
  am_inf_am_parameters params = {uint64_t(win), uint64_t(chunk_size), uint8_t(dimension), {0}};
  amoba.init_fun(&amoba, &params);

  SECTION("setting only one cell") {
    am_inf_am_position p = position(uint8_t(dimension));
    amoba.set_fun(&amoba, 1, &p);
    REQUIRE(amoba.check_fun(&amoba, &p) == false);
  }
  SECTION ("testing win condition") {
    am_inf_am_position p = position(uint8_t(dimension));
    for (int i = 0; i < win; i++) {
      p.component[0]++;
      amoba.set_fun(&amoba, 1, &p);
    }
    REQUIRE(amoba.check_fun(&amoba, &p) == true);
  }
  SECTION ("testing losing") {
    am_inf_am_position p = position(uint8_t(dimension));
    p.component[0] = 0;
    amoba.set_fun(&amoba, 2, &p);
    p.component[0] = 1;
    REQUIRE(amoba.check_fun(&amoba, &p) == false);
  }
  SECTION ("testing big cell numbers") {
    am_inf_am_position p = position(uint8_t(dimension));
    auto x = GENERATE(take(100, random(-1000000000, 1000000000)));
    auto y = GENERATE(take(100, random(-1000000000, 1000000000)));
    p.component[0] = x;
    amoba.set_fun(&amoba, y, &p);
    REQUIRE(amoba.get_fun(&amoba, &p) == y);
  }
}
