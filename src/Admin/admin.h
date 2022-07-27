#ifndef ADMIN_ADMIN_H
#define ADMIN_ADMIN_H

#include <Windows.h>
#include <tchar.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <map>
#include <string>
#include <vector>

#include "network_data.h"

#pragma comment(lib, "Ws2_32.lib")

class Admin {
  public:
  Admin();

  // for error checking return values
  int iResult;

  //main thing
  // continue checking and sending to client
  void update();

  //receive message from clients
  void receiveFromClients();

  //send message to server
  void sendActionPackets();

  void serverAssign();
  void connectWrite();
  void connectRead();
  void init();
  void receive(char *recvbuf);
  void send(char *packets, int totalSize);

  const int DEFAULT_BUFLEN = 512;
  const char *DEFAULT_PORT = "6881";
  bool is_send;
  static int total_process;
  static int count_down;

  private:
  HANDLE admin_Write_Pipe;
  HANDLE admin_Read_Pipe;

  char network_data[MAX_PACKET_SIZE];
  char *send_content;
  int client_id;
};
#endif
