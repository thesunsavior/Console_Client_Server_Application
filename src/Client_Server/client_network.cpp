#include "client_network.h"

#include <tchar.h>

#include <iostream>

ClientNetwork::ClientNetwork(bool is_server) {
  this->is_server = is_server;
  if (!is_server) {
    pipe_controller.init();
  }
}

ClientNetwork::~ClientNetwork() = default;

int ClientNetwork::receivePackets(char* recvbuf) {
  if (is_server) return 0;
  pipe_controller.receive(recvbuf);
  return iResult;
}

void ClientNetwork::sendActionPackets() {
  if (is_server) return;

  const unsigned int packet_size = sizeof(Packet);
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


  std::cout << "Client-time report: " << network_data << std::endl;
}

bool ClientNetwork::isServer() {
  return is_server;
}

void ClientNetwork::makeServer() {
  this->is_server = true;
  CloseHandle(pipe_controller.pipe);
  CloseHandle(pipe_controller.admin_read_pipe);
  CloseHandle(pipe_controller.admin_write_pipe);

  this->server = new ServerNetwork(pipe_controller.admin_write_pipe, pipe_controller.admin_read_pipe);
}

void ClientNetwork::resolveAdminData() {
  pipe_controller.receiveFromAdmin(network_data);


  if (strcmp(network_data, "ACTION_EVENT") == 0) {
    std::cout << "Client receive server assignment" << std::endl;

    this->makeServer();
  } else {
    std::cout << "Client received weird message:" << network_data << std::endl;
  }
}
