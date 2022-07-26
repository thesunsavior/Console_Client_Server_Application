#ifndef CLIENTSERVER_SERVERPIPE_H
#define CLIENTSERVER_SERVERPIPE_H

#include <unistd.h>
#include <winsock2.h>

#include "pipe.h"

class ServerPipe : public Pipe {
  public:
  void init(HANDLE cadmin_write_pipe, HANDLE cadmin_read_pipe);
  void send(char *packets, int totalSize) override;
  void receive(char *recvbuf) override{};
  void connect();

  boolean connectToAdmin();
  void sendToAdmin(char *packets, int totalSize);
  void signalActiveAdmin();
};
#endif//FINALTASK_SERVERPIPE_H
