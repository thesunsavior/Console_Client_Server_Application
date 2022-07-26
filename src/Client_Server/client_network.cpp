#include "client_network.h"

#include <tchar.h>

#include <iostream>

#include "Logger.h"

ClientNetwork::ClientNetwork(bool is_server) {
  this->is_server = is_server;

  if (!is_server) {
    pipe_controller.init();
  }
}

ClientNetwork::~ClientNetwork() = default;

int32_t ClientNetwork::receivePackets(char* recvbuf) {
  if (is_server) return 0;
  pipe_controller.receive(recvbuf);
  return iResult;
}

void ClientNetwork::sendActionPackets() {
  if (is_server) return;

  const uint32_t packet_size = sizeof(Packet);
  char packet_data[packet_size];

  Packet packet;
  packet.packet_type = ACTION_EVENT;

  packet.serialize(packet_data);

  pipe_controller.send(packet_data, packet_size);
}

void ClientNetwork::update() {
  if (is_server) return;

  memset(network_data, 0, sizeof network_data);
  resolveAdminData();

  //check on server
  if (is_server) return;

  memset(network_data, 0, sizeof network_data);
  receivePackets(network_data);

  LOG(DEBUG) << "Client-time report: " << network_data;
  std::cout << "Client-time report: " << network_data << std::endl;
  CloseHandle(ClientPipe::admin_read_pipe);
  CloseHandle(ClientPipe::admin_write_pipe);
}

bool ClientNetwork::isServer() {
  return is_server;
}

void ClientNetwork::makeServer() {
  this->is_server = true;
  CloseHandle(ClientPipe::pipe);
  CloseHandle(ClientPipe::admin_read_pipe);
  CloseHandle(ClientPipe::admin_write_pipe);
  LOG(INFO) << "***************************************** SERVER INIT ****************************************";
}

void ClientNetwork::resolveAdminData() {
  pipe_controller.receiveFromAdmin(network_data);


  if (strcmp(network_data, "ACTION_EVENT") == 0) {
    LOG(INFO) << "Client receive server assignment";

    this->makeServer();
  } else {
    LOG(DEBUG) << "Client received weird message:" << network_data;
  }
}
