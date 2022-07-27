#include "server_network.h"

#include <tchar.h>

#include <ctime>
#include <iostream>
#include <thread>


ServerNetwork::ServerNetwork() {}

// send data to all clients
void ServerNetwork::sendToAll(char* packets, int32_t totalSize) {
  ServerPipe::send(packets, totalSize);
}


void ServerNetwork::receiveFromClients() {
}


void ServerNetwork::sendActionPackets() {
  while (true){
    //get current time
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);

    sendToAll(buffer, sizeof(buffer));
  }

}


void ServerNetwork::update() {
  ServerPipe::init();
  bool connection = ServerPipe::connect();

  if (!connection) {
    return;
  }

  std::thread t1(&ServerNetwork::sendActionPackets);

  t1.join();
}
