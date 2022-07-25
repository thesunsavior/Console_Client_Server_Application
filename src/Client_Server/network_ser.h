  //
// Created by Trung on 7/18/2022.
//

#ifndef TASK7_NETWORKSER_H
#define TASK7_NETWORKSER_H
#include <winsock2.h>
#include <Windows.h>

class NetworkServices
{
public:
  static int sendMessage(SOCKET curSocket, char * message, int messageSize);
  static int receiveMessage(SOCKET curSocket, char * buffer, int bufSize);
};

#endif //TASK7_NETWORKSER_H
