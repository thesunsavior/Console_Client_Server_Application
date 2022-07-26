//
// Created by Trung on 7/18/2022.
//

#ifndef CLIENTSERVER_NETWORKSER_H
#define CLIENTSERVER_NETWORKSER_H
#include <Windows.h>
#include <winsock2.h>

class NetworkServices {
  public:
  static int sendMessage(SOCKET curSocket, char* message, int messageSize);
  static int receiveMessage(SOCKET curSocket, char* buffer, int bufSize);
};

#endif//TASK7_NETWORKSER_H
