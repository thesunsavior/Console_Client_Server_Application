#include <iostream>
#include <vector>

#include "client_network.h"

int main() {
  //write log file
  freopen("Client_Server_error.txt", "w", stderr);

  std::cerr << "---Client-Server activity---" << std::endl;

  auto *client = new ClientNetwork(false);// argument 2 is IPC type: 0 for socket 1 for pipe

  while (true) {
    if (client->isServer()) {
      ServerNetwork::update();
    } else {
      client->update();
    }
  }


  return 0;
}
