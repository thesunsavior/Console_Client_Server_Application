#include "server_network.h"

#include <tchar.h>
#include <winbase.h>

#include <ctime>
#include <iostream>


unsigned int ServerNetwork::client_id;

ServerNetwork::ServerNetwork(HANDLE cadmin_write_pipe, HANDLE cadmin_read_pipe) {
  std::cout<<"***************************************** SERVER INIT ****************************************"<<std::endl;
  ServerNetwork::client_id = 0;


  pipe_controller.init(cadmin_write_pipe, cadmin_read_pipe);

  //  pipe_controller.signalActiveAdmin();
}

// send data to all clients
void ServerNetwork::sendToAll(char* packets, int totalSize) {
  pipe_controller.send(packets, totalSize);
}


void ServerNetwork::receiveFromClients() {
  pipe_controller.connect();
  sendActionPackets();
}


void ServerNetwork::sendActionPackets() {
  //get current time
  time_t rawtime;
  struct tm* timeinfo;
  char buffer[80];

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);

  sendToAll(buffer, sizeof(buffer));
}


void ServerNetwork::update() {
  receiveFromClients();
}
