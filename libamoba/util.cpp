#include "util.hpp"
uint64_t pow(uint64_t base, uint64_t exp) {
  uint64_t result = 1;
  while (exp) {
    if (exp % 2)
      result *= base;
    exp /= 2;
    base *= base;
  }
  return result;
}
