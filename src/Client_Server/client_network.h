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
#include "server_network.h"

class ClientNetwork {
  public:
  const char *const DEFAULT_PORT = "6881";

  int32_t iResult{};

  void sendActionPackets();

  void update();

  int32_t receivePackets(char *);
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


#endif
