#ifndef CLIENTSERVER_PIPE_H
#define CLIENTSERVER_PIPE_H

#include <tchar.h>
#include <winbase.h>

#include "IPC.h"
#include "network_data.h"

class Pipe : public IPC {
  public:
  static void send(char *packets, int32_t totalSize){};
  static void receive(char *recvbuf){};
  static void init(){};

  Pipe() = default;
  ~Pipe() = default;

  static int32_t iResult;
  static HANDLE pipe;
  static HANDLE admin_write_pipe;
  static HANDLE admin_read_pipe;
};

#endif
