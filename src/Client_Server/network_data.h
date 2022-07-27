//
// Created by Trung on 7/18/2022.
//

#ifndef CLIENTSERVER_NETWORKDATA_H
#define CLIENTSERVER_NETWORKDATA_H

#include <string.h>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

  INIT_CONNECTION = 0,

  ACTION_EVENT = 1,

  DEAD_SERVER = 2,

};

class Packet {
  public:
  uint32_t packet_type;

  void serialize(char* data) {
    memcpy(data, this, sizeof(Packet));
  }

  void deserialize(char* data) {
    memcpy(this, data, sizeof(Packet));
  }
};
#endif
