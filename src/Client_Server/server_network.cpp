#include "server_network.h"

#include <tchar.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

ServerNetwork::ServerNetwork() {}

std::string serializeTimePoint(const TimePoint &time,
                               const std::string &format) {
  std::time_t tt = std::chrono::system_clock::to_time_t(time);
  std::tm tm = *std::gmtime(&tt);// GMT (UTC)
  std::stringstream ss;
  ss << std::put_time(&tm, format.c_str());
  return ss.str();
}

// send data to all clients
void ServerNetwork::sendToAll(char *packets, int32_t totalSize, HANDLE myPipe) {
  ServerPipe::send(packets, totalSize, myPipe);
}


void ServerNetwork::receiveFromClients() {
}


void ServerNetwork::sendActionPackets(HANDLE myPipe) {
  while (true) {
    //get current time
    std::chrono::time_point t = std::chrono::system_clock::now();
    std::string crrTime = serializeTimePoint(t, "UTC: %Y-%m-%d %H:%M:%S");

    char buffer[80];
    strcpy(buffer,crrTime.c_str());

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
