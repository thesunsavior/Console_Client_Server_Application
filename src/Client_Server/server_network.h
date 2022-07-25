#ifndef TASK7_SERVERNETWORK_H
#define TASK7_SERVERNETWORK_H

#include <winsock2.h>
#include <Windows.h>
#include <unistd.h>
#include <tchar.h>
#include <ws2tcpip.h>

#include <map>

#include "network_data.h"
#include "network_ser.h"
#include "server_pipe.h"

#pragma comment (lib, "Ws2_32.lib")

class ServerNetwork
{
public:
  const char* DEFAULT_PORT = "6881";
  const int DEFAULT_BUFLEN = 512;

  // helper function
  // send data to all clients
  void sendToAll(char * packets, int totalSize);

  // receive incoming data
  int receiveData(unsigned int client_id, char * recvbuf);

  // accept new connections
  bool acceptNewClient(unsigned int & id);


  // for error checking return values
  int iResult;



  //main thing
  // continue checking and sending to client
  void update();


  //receive message from clients
  void receiveFromClients();

  //send message to server
  void sendActionPackets();

  //*************** SOCKET ********************

  //socket init
  // Socket to listen for new connections
  SOCKET ListenSocket;

  // Socket to give to the clients
  SOCKET ClientSocket;

  // table to keep track of each client's socket
  std::map<unsigned int, SOCKET> sessions;

  void sockInit();
  void sockSend(char * packets, int totalSize);
  void sockReceive();

  //type is type of ipc to use: 0->socket, 1-> pipe
  ServerNetwork(int type,HANDLE cadmin_write_pipe, HANDLE cadmin_read_pipe);
  ~ServerNetwork();
private:
  ServerPipe pipe_controller;
  static unsigned int client_id ;
  int IPC_type;
  char network_data[MAX_PACKET_SIZE];

};
#endif //TASK7_SERVERNETWORK_H
