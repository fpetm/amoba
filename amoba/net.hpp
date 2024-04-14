#pragma once

enum PacketType {
  None,
  Hello, Goodbye,
};

struct Packet {
  PacketType type;
  size_t size;
  void *data;
};

namespace Packets {
struct Hello {
  char name[256];
  int length;
};

struct Goodbye {
};
}
