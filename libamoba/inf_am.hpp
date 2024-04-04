#pragma once
#include <amoba/amoba.h>
#include <amoba/amobas/inf_am.h>
#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "util.hpp"

class Position {
public:
  Position(am_inf_am_position p);
  Position(uint8_t dimension, std::vector<int64_t> data)
      : m_Dimension(dimension), m_Components(data){};
  Position(uint8_t dimension);

  int64_t &operator[](uint8_t n) { return m_Components[n]; }
  int64_t operator[](uint8_t n) const { return m_Components[n]; }

  Position operator+(Position) const;
  Position operator-(Position) const;
  Position operator*(int64_t) const;
  Position operator/(int64_t) const;
  bool operator<(Position) const;
  bool operator==(Position) const;
  friend std::ostream &operator<<(std::ostream &, const Position &);

public:
  std::vector<int64_t> m_Components;
  uint8_t m_Dimension;
};

extern std::vector<Position> g_Offsets[];

struct Chunk {
  Chunk(uint64_t chunk_size, uint8_t dimensions, Position offset)
      : m_ChunkSize(chunk_size), m_Dimensions(dimensions),
        m_Data(pow(chunk_size, dimensions), 0), m_ChunkOffset(offset),
        m_Offset(offset * chunk_size) {}
  uint64_t m_ChunkSize;
  uint8_t m_Dimensions;

  Position m_ChunkOffset; // offset in chunk space
  Position m_Offset;      // offset in absolute space
  std::vector<color_t> m_Data;

public:
  int64_t index_from_abs_pos(const Position pos) const;
  color_t &at(const Position pos) { return m_Data[index_from_abs_pos(pos)]; }
  const color_t &at(const Position pos) const {
    return m_Data[index_from_abs_pos(pos)];
  }
  color_t &operator[](const Position pos) { return at(pos); }
  const color_t &operator[](const Position pos) const { return at(pos); }
};

struct Board {
  std::vector<std::shared_ptr<Chunk>> m_Chunks;
  uint8_t m_Dimension;
  uint64_t m_ChunkSize;
  uint64_t m_Win;
  bool m_Infinite;
  Position m_Size;

  Board(uint8_t dimension, uint64_t chunk_size, uint64_t win, uint64_t size[4])
      : m_Dimension(dimension), m_ChunkSize(chunk_size), m_Win(win),
        m_Infinite(size[0] == 0), m_Size(dimension) {
    for (uint8_t i = 0; i < m_Dimension; i++)
      m_Size[i] = size[i];
  }

  color_t &at(const Position pos);
  color_t &operator[](const Position pos) { return at(pos); }
  color_t check_for_win(Position pos);

  void clear();
};

void init(am_amoba *amoba, void *vparams);
void destroy(am_amoba *amoba, void *);
color_t get(am_amoba *amoba, void *mp);
void set(am_amoba *amoba, color_t color, void *mp);
color_t check(am_amoba *amoba, void *mp);

extern "C" am_amoba am_construct_inf_am(void);
