//
// Created by Trung on 7/18/2022.
//

#ifndef TASK7_CLIENTNETWORK_H
#define TASK7_CLIENTNETWORK_H

#include <winsock2.h>
#include <Windows.h>
#include <ws2tcpip.h>

#include <chrono>
#include <thread>
#include <cstdio>
#include <string>

#include "network_ser.h"
#include "network_data.h"
#include "server_network.h"
#include "client_pipe.h"

// port to connect sockets through
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

class ClientNetwork
{
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
  void makeServer ();
  bool isServer() ;
  void resolveAdminData();

  //*********SOCKET********
  void sockInit();
  void sockSend(char *packet_data,unsigned int packet_size);
  void sockReceive(char * recvbuf);

  //*********pipe********
//    HANDLE pipe;
//    HANDLE adminPipe;
//
//    void pipeInit();
//    void pipeSend(char * packets, int totalSize);
//    void pipeReceive(char * recvbuf);
//
//    void pipeSendToAdmin(char *packets, int totalSize);
//    void sendPackageToAdmin(PacketTypes type, int id);
//    void pipeReceiveFromAdmin(char *recvbuf);
//    void pipeReadAdmin();

  ClientNetwork(bool isServer,int type);
  ~ClientNetwork();

private:
  bool is_server;
  int IPC_type;
  char network_data[MAX_PACKET_SIZE]{};
  ClientPipe pipe_controller;
};


#endif //TASK7_CLIENTNETWORK_H
