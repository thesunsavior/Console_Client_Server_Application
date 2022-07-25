//
// Created by trungpq on 7/22/2022.
//

#ifndef FINALTASK_IPC_H
#define FINALTASK_IPC_H

class IPC {
public:
  virtual void init()=0;
  virtual void send(char *packets, int totalSize)=0;
  virtual void receive(char * recvbuf)=0;
};

#endif //FINALTASK_IPC_H
