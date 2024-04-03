#ifndef INCLUDE_AMOBA_AMOBA_H
#define INCLUDE_AMOBA_AMOBA_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t color_t;

typedef struct am_amoba am_amoba;

typedef void(am_amoba_init_fun)(am_amoba *, void *);
typedef void(am_amoba_destroy_fun)(am_amoba *, void *);
typedef color_t(am_amoba_get_fun)(am_amoba *, void *);
typedef void(am_amoba_set_fun)(am_amoba *, color_t, void *);
typedef color_t(am_amoba_check_fun)(am_amoba *, void *);

typedef struct am_amoba {
  const char *name;
  am_amoba_init_fun *init_fun;
  am_amoba_destroy_fun *destroy_fun;
  am_amoba_get_fun *get_fun;
  am_amoba_set_fun *set_fun;
  am_amoba_check_fun *check_fun;
  void *data;
} am_amoba;

extern am_amoba am_construct_inf_am(void);
static am_amoba am_amoba_list[] = {am_construct_inf_am(), NULL};

#ifdef __cplusplus
};
#endif
#endif
