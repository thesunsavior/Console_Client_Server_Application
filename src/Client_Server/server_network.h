#ifndef CLIENTSERVER_SERVERNETWORK_H
#define CLIENTSERVER_SERVERNETWORK_H

#include <Windows.h>
#include <tchar.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <map>

#include "network_data.h"
#include "network_ser.h"
#include "server_pipe.h"

#pragma comment(lib, "Ws2_32.lib")

class ServerNetwork {
  public:
  const char* DEFAULT_PORT = "6881";
  const int DEFAULT_BUFLEN = 512;

  // helper function
  // send data to all clients
  void sendToAll(char* packets, int totalSize);

  // receive incoming data
  int receiveData(unsigned int client_id, char* recvbuf);


  // for error checking return values
  int iResult;


  //main thing
  // continue checking and sending to client
  void update();


  //receive message from clients
  void receiveFromClients();

  //send message to server
  void sendActionPackets();


  ServerNetwork(HANDLE cadmin_write_pipe, HANDLE cadmin_read_pipe);
  ~ServerNetwork();

  private:
  ServerPipe pipe_controller;
  static unsigned int client_id;
  char network_data[MAX_PACKET_SIZE];
};
#endif//TASK7_SERVERNETWORK_H
