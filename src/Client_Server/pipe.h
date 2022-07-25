#ifndef FINALTASK_PIPE_H
#define FINALTASK_PIPE_H

#include <winbase.h>
#include <tchar.h>

#include "IPC.h"
#include "network_data.h"

class Pipe : public IPC{
public:
  virtual void send(char *packets, int totalSize)=0;
  virtual void receive(char * recvbuf)=0;
  virtual void init(){};

  Pipe()= default;
  ~Pipe()= default;

  int iResult;
  HANDLE pipe;
  HANDLE admin_write_pipe;
  HANDLE admin_read_pipe;
};

#endif //FINALTASK_PIPE_H
