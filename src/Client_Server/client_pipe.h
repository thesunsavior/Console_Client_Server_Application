#ifndef CLIENTSERVER_CLIENTPIPE_H
#define CLIENTSERVER_CLIENTPIPE_H

#include <winsock2.h>

#include <string>

#include "network_data.h"
#include "pipe.h"
#include "server_network.h"

class ClientPipe : public Pipe {
  public:
  void init() ;
  void send(char *packets, int32_t totalSize) {};
  void receive(char *recvbuf);

  bool connectToAdmin();
  bool connectToAdminRead();
  bool connectToAdminWrite();
  void sendToAdmin(char *packets, int totalSize);

  void sendPackageToAdmin(char *type);
  void receiveFromAdmin(char *recvbuf);

  int32_t iResult;
  static HANDLE pipe;
  static HANDLE admin_write_pipe;
  static HANDLE admin_read_pipe;

  ClientPipe() {}
  ~ClientPipe() {}
};


#endif
