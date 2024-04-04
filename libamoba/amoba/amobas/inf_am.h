#ifndef INCLUDE_AMOBA_AMOBAS_INF_AM_H
#define INCLUDE_AMOBA_AMOBAS_INF_AM_H
#include <stdint.h>

typedef struct am_inf_am_parameters {
  uint64_t win_n;
  uint64_t chunk_size;
  uint8_t dimension;
  uint64_t max_sizes[4];
} am_inf_am_parameters;

typedef struct am_inf_am_position {
  uint64_t component[4];
  uint8_t dimension;
} am_inf_am_position;

#endif
