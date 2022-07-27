#include "server_network.h"

#include <tchar.h>

#include <ctime>
#include <iostream>
#include <thread>


ServerNetwork::ServerNetwork() {}

// send data to all clients
void ServerNetwork::sendToAll(char* packets, int32_t totalSize, HANDLE myPipe) {
  ServerPipe::send(packets, totalSize, myPipe);
}


void ServerNetwork::receiveFromClients() {
}


void ServerNetwork::sendActionPackets(HANDLE myPipe) {
  while (true) {
    //get current time
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);

    sendToAll(buffer, sizeof(buffer), myPipe);
  }
}


void ServerNetwork::update() {
  ServerPipe::init();
  bool connection = ServerPipe::connect();

  if (!connection) {
    return;
  }

  std::thread t1(&ServerNetwork::sendActionPackets, ServerPipe::pipe);

  t1.detach();
  sleep(1);
}
