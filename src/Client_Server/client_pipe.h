#ifndef FINALTASK_CLIENTPIPE_H
#define FINALTASK_CLIENTPIPE_H

#include <string>

#include "server_network.h"
#include "network_data.h"
#include "pipe.h"

class ClientPipe: public Pipe{
public:
  void init() override;
  void send(char * packets, int totalSize) override{};
  void receive(char * recvbuf) override;

  boolean connectToAdmin();
  boolean connectToAdminRead();
  boolean connectToAdminWrite();
  void sendToAdmin(char *packets, int totalSize);
  std::string GetLastErrorAsString();

  //  void sendPackageToAdmin(PacketTypes type, int id);
  void sendPackageToAdmin(char* type);
  void receiveFromAdmin(char *recvbuf);

   ClientPipe(){}
   ~ClientPipe(){}
};


#endif //FINALTASK_CLIENTPIPE_H
