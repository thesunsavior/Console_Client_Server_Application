#ifndef CLIENTSERVER_IPC_H
#define CLIENTSERVER_IPC_H

class IPC {
  public:
  static void init(){};
  static void send(char *packets, int totalSize){};
  static void receive(char *recvbuf){};
};

#endif
