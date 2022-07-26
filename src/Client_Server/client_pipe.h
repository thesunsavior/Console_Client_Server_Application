#ifndef CLIENTSERVER_CLIENTPIPE_H
#define CLIENTSERVER_CLIENTPIPE_H

#include <winsock2.h>

#include <string>

#include "network_data.h"
#include "pipe.h"
#include "server_network.h"

class ClientPipe : public Pipe {
  public:
  void init() override;
  void send(char *packets, int totalSize) override{};
  void receive(char *recvbuf) override;

  boolean connectToAdmin();
  boolean connectToAdminRead();
  boolean connectToAdminWrite();
  void sendToAdmin(char *packets, int totalSize);

  void sendPackageToAdmin(char *type);
  void receiveFromAdmin(char *recvbuf);

  ClientPipe() {}
  ~ClientPipe() {}
};


#endif
