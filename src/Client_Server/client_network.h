#ifndef CLIENTSERVER_CLIENTNETWORK_H
#define CLIENTSERVER_CLIENTNETWORK_H

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <chrono>
#include <cstdio>
#include <string>
#include <thread>

#include "client_pipe.h"
#include "network_data.h"
#include "network_ser.h"
#include "server_network.h"

// port to connect sockets through
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

class ClientNetwork {
  public:
  const char *const DEFAULT_PORT = "6881";

  ServerNetwork *server{};

  // for error checking function calls in Winsock library
  int iResult{};

  // socket for client to connect to server
  SOCKET ConnectSocket{};

  void sendActionPackets();

  void update();

  int receivePackets(char *);
  void makeServer();
  bool isServer();
  void resolveAdminData();

  ClientNetwork(bool isServer);
  ~ClientNetwork();

  private:
  bool is_server;
  char network_data[MAX_PACKET_SIZE];
  ClientPipe pipe_controller;
};


#endif//TASK7_CLIENTNETWORK_H
