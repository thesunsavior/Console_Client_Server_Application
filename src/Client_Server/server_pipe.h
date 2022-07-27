#ifndef CLIENTSERVER_SERVERPIPE_H
#define CLIENTSERVER_SERVERPIPE_H

#include <unistd.h>
#include <winsock2.h>

#include <cstdint>

#include "pipe.h"

class ServerPipe {
  public:
  static void init();
  static void send(char *packets, int totalSize);
  static void receive(char *recvbuf){};
  static bool connect();

  static boolean connectToAdmin();
  static void sendToAdmin(char *packets, int totalSize);
  void signalActiveAdmin();

  static int32_t iResult;
  static HANDLE pipe;
  static HANDLE admin_write_pipe;
  static HANDLE admin_read_pipe;
};
#endif//FINALTASK_SERVERPIPE_H
