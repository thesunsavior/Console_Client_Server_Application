#ifndef CLIENTSERVER_SERVERNETWORK_H
#define CLIENTSERVER_SERVERNETWORK_H
#include <winsock2.h>
#include <Windows.h>
#include <tchar.h>
#include <unistd.h>
#include <ws2tcpip.h>

#include <map>

#include "network_data.h"
#include "server_pipe.h"

#pragma comment(lib, "Ws2_32.lib")

class ServerNetwork {
  public:
  const char* DEFAULT_PORT = "6881";
  const int DEFAULT_BUFLEN = 512;

  // helper function
  // send data to all clients
  static void sendToAll(char* packets, int totalSize);

  //main thing
  // continue checking and sending to client
  static void update();


  //receive message from clients
  static void receiveFromClients();

  //send message to server
  static void sendActionPackets();


  ServerNetwork();
  ~ServerNetwork();
};
#endif
