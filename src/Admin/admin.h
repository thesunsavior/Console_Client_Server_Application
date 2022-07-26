#ifndef FINALTASK_ADMIN_H
#define FINALTASK_ADMIN_H

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <unistd.h>
#include <tchar.h>

#include <vector>
#include <map>
#include <string>

#include "network_data.h"

#pragma comment (lib, "Ws2_32.lib")

class Admin{
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
private:
  HANDLE admin_Write_Pipe;
  HANDLE admin_Read_Pipe;

  char network_data[MAX_PACKET_SIZE];
  char* send_content;
  boolean is_send;
  int client_id;
  int total_process;
  int total_pipe;
  int count_down;
};
#endif //FINALTASK_ADMIN_H
