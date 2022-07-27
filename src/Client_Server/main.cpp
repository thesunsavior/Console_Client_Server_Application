#include <vector>

#include "client_network.h"

#pragma comment(lib, "Ws2_32.lib")


int main() {
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
