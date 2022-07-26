//
// Created by trungpq on 7/22/2022.
//

#ifndef CLIENTSERVER_IPC_H
#define CLIENTSERVER_IPC_H

class IPC {
  public:
      static void init() {};
      static void send(char *packets, int totalSize) {};
      static void receive(char *recvbuf) {};
};

#endif//FINALTASK_IPC_H
