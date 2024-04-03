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
  Position(am_inf_am_position p) {
    m_Dimension = p.dimension;
    m_Components.resize(m_Dimension);
    for (uint8_t i = 0; i < m_Dimension; i++) {
      m_Components[i] = p.component[i];
    }
  };
  Position(uint8_t dimension, std::vector<int64_t> data)
      : m_Dimension(dimension), m_Components(data){};

  Position(uint8_t dimension) : m_Dimension(dimension) {
    m_Components.resize(dimension);
    for (int i = 0; i < dimension; i++)
      m_Components[i] = 0;
  }

  int64_t operator[](uint8_t n) const { return m_Components[n]; }

  Position operator+(const Position b) const {
    Position c(std::max(m_Dimension, b.m_Dimension));
    for (uint8_t i = 0; i < std::min(m_Dimension, b.m_Dimension); i++) {
      c.m_Components[i] = m_Components[i] + b[i];
    }
    return c;
  };
  Position operator-(const Position b) const {
    Position c(std::max(m_Dimension, b.m_Dimension));
    for (uint8_t i = 0; i < std::min(m_Dimension, b.m_Dimension); i++) {
      c.m_Components[i] = m_Components[i] - b[i];
    }
    return c;
  };
  Position operator*(int64_t a) const {
    Position c(m_Dimension);
    for (uint8_t i = 0; i < m_Dimension; i++) {
      c.m_Components[i] = m_Components[i] * a;
    }
    return c;
  };
  Position operator/(int64_t a) const {
    Position c(m_Dimension);
    for (uint8_t i = 0; i < m_Dimension; i++) {
      c.m_Components[i] = int(floor(m_Components[i] / (double)a));
    }
    return c;
  };
  bool operator==(Position b) {
    if (m_Dimension != b.m_Dimension)
      return false;
    for (uint8_t i = 0; i < m_Dimension; i++) {
      if (m_Components[i] != b[i])
        return false;
    }
    return true;
  }
  friend std::ostream &operator<<(std::ostream &os, const Position &p) {
    os << "(";
    for (auto p : p.m_Components) {
      os << p << ", ";
    }
    os << ")";
    return os;
  }

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

  color_t &at(const Position pos) {
    Position relative_position = pos - m_Offset;
    int64_t index = 0;
    int64_t base = 1;
    for (uint8_t i = 0; i < m_Dimensions; i++) {
      index += base * relative_position[i];
      base *= m_ChunkSize;
    };
    return m_Data[index];
  }
  const color_t &at(const Position pos) const {
    Position relative_position = pos - m_Offset;
    int index = 0;
    int base = 1;
    for (uint8_t i = 0; i < m_Dimensions; i++) {
      index += base * pos[i];
      base *= m_ChunkSize;
    };
    return m_Data[index];
  }
  color_t &operator[](const Position pos) { return at(pos); }
  const color_t &operator[](const Position pos) const { return at(pos); }
};

struct Board {
  std::vector<std::shared_ptr<Chunk>> m_Chunks;
  uint8_t m_Dimension;
  uint64_t m_ChunkSize;
  uint64_t m_Win;
  Board(uint8_t dimension, uint64_t chunk_size, uint64_t win)
      : m_Dimension(dimension), m_ChunkSize(chunk_size), m_Win(win) {}

  color_t &at(const Position pos) {
    for (auto &chunk : m_Chunks) {
      if (pos / m_ChunkSize == chunk->m_ChunkOffset) {
        return chunk->at(pos);
      }
    }
    std::shared_ptr<Chunk> chunk =
        std::make_shared<Chunk>(m_ChunkSize, m_Dimension, pos / m_ChunkSize);
    m_Chunks.emplace_back(chunk);
    return chunk->at(pos);
  }
  color_t &operator[](const Position pos) { return at(pos); }
  color_t check_for_win(Position pos) {
    color_t c = at(pos);
    for (Position offset : g_Offsets[m_Dimension]) {
      bool won = true;
      for (int n = 1; n < m_Win; n++) {
        if (at(pos + offset * n) != c) {
          won = false;
        }
      }
      if (won)
        return c;
    }
    return 0;
  }
};

void init(am_amoba *amoba, void *vparams) {
  am_inf_am_parameters *params = (am_inf_am_parameters *)vparams;
  amoba->data = new Board(params->dimension, params->chunk_size, params->win_n);
};
void destroy(am_amoba *amoba, void *) { delete (Board *)amoba->data; }
color_t get(am_amoba *amoba, void *mp) {
  Position pos(*(am_inf_am_position *)mp);
  Board *b = (Board *)amoba->data;
  return b->at(pos);
}
void set(am_amoba *amoba, color_t color, void *mp) {
  Position pos(*(am_inf_am_position *)mp);
  Board *b = (Board *)amoba->data;
  b->at(pos) = color;
}
color_t check(am_amoba *amoba, void *mp) {
  Position pos(*(am_inf_am_position *)mp);
  Board *b = (Board *)amoba->data;
  return b->check_for_win(pos);
}

extern "C" am_amoba am_construct_inf_am(void) {
  am_amoba inf_am;
  inf_am.init_fun = &init;
  inf_am.get_fun = &get;
  inf_am.set_fun = &set;
  inf_am.check_fun = &check;
  inf_am.destroy_fun = &destroy;
  return inf_am;
}
