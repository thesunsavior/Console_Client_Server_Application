#ifndef ADMIN_NETWORKDATA_H
#define ADMIN_NETWORKDATA_H

#include <string.h>

#define MAX_PACKET_SIZE 1000000

enum PacketTypes {

  INIT_CONNECTION = 0,

  ACTION_EVENT  = 1,

  DEAD_SERVER   = 2,

};

class Packet {
public:
  unsigned int packet_type;
  unsigned int id;

  void serialize(char * data) {
      memcpy(data, this, sizeof(Packet));
  }

  void deserialize(char * data) {
      memcpy(this, data, sizeof(Packet));
  }
};
#endif //TASK7_NETWORKDATA_H
