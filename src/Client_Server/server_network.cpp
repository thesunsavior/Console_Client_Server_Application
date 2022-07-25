#include "server_network.h"

#include <winbase.h>
#include <tchar.h>

#include <iostream>
#include <ctime>


unsigned int ServerNetwork::client_id ;

ServerNetwork::ServerNetwork(int type,HANDLE cadmin_write_pipe, HANDLE cadmin_read_pipe)
{
  printf("***************************************** SERVER INIT ****************************************\n");
  ServerNetwork::client_id=0;


  this->IPC_type = type;

  if (type == 0) {
    sockInit();
  } else { pipe_controller.init(cadmin_write_pipe,cadmin_read_pipe); }

//  pipe_controller.signalActiveAdmin();
}

// send data to all clients
void ServerNetwork::sendToAll(char * packets, int totalSize){

  if (IPC_type == 0) {
      sockSend(packets, totalSize);
  } else { pipe_controller.send(packets, totalSize); }

}


void ServerNetwork::receiveFromClients() {
  if(IPC_type == 0) {
      sockReceive();
  } else
  {
      pipe_controller.connect();
      sendActionPackets();
  }
}



void ServerNetwork::sendActionPackets()
{
  //get current time
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];

  time (&rawtime);
  timeinfo = localtime(&rawtime);

  strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);

  sendToAll(buffer,sizeof(buffer));
}


void ServerNetwork::update(){
  if (IPC_type ==0)
  {
      if(acceptNewClient(ServerNetwork::client_id)){
          printf("New client %d connected.\n",ServerNetwork::client_id);
      }
  }

  receiveFromClients();
}



//*************************************************SOCKET************************************************************


void ServerNetwork::sockInit() {


  // create WSADATA object
  WSADATA wsaData;

  // sockets for the server
  ListenSocket = INVALID_SOCKET;
  ClientSocket = INVALID_SOCKET;



  // address info for the server to listen to
  struct addrinfo *result = nullptr;
  struct addrinfo hints;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
  if (iResult != 0) {
      std::cout << "WSAStartup failed with error: "<< iResult<<std::endl;
      exit(1);
  }
  std::cout<<"successful init winsock"<<std::endl ;

  // set address information
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;    // TCP connection!!!
  hints.ai_flags = AI_PASSIVE;

  // Resolve the server address and port
  iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);

  if ( iResult != 0 ) {
      printf("getaddrinfo failed with error: %d\n", iResult);
      WSACleanup();
      exit(1);
  }

  // Create a SOCKET for connecting to server
  std::cout << "Creating listening socket... ";
  ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (ListenSocket == INVALID_SOCKET) {
      printf("socket failed with error: %ld\n", WSAGetLastError());
      freeaddrinfo(result);
      WSACleanup();
      exit(1);
  }
  std::cout << "Success!!! \n";

  std::cout << "Settiing nonblocking socket... ";
  // Set the mode of the socket to be nonblocking
  u_long iMode = 1;
  iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);

  if (iResult == SOCKET_ERROR) {
      printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
      closesocket(ListenSocket);
      WSACleanup();
      exit(1);
  }

  std::cout << "Success!!! \n";

  // Setup the TCP listening socket
  std::cout << "Binding socket... ";
  iResult = bind( ListenSocket, result->ai_addr, static_cast<int>(result->ai_addrlen));

  if (iResult == SOCKET_ERROR) {
      printf("bind failed with error: %d\n", WSAGetLastError());
      freeaddrinfo(result);
      closesocket(ListenSocket);
      WSACleanup();
      exit(1);
  }
  std::cout << "Success!!! \n";


  // no longer need address information
  freeaddrinfo(result);

  // start listening for new clients attempting to connect
  std::cout << "Putting socket in listening mode... ";
  iResult = listen(ListenSocket, SOMAXCONN);

  if (iResult == SOCKET_ERROR) {
      printf("listen failed with error: %d\n", WSAGetLastError());
      closesocket(ListenSocket);
      WSACleanup();
      exit(1);
  }

  std::cout << "Success!!! \n";

}

// receive incoming data
int ServerNetwork::receiveData(unsigned int client_id, char * recvbuf){
  if( sessions.find(client_id) != sessions.end() )
  {
      SOCKET currentSocket = sessions[client_id];
      iResult = NetworkServices::receiveMessage(currentSocket, recvbuf, MAX_PACKET_SIZE);

      if (iResult == 0)
      {
          printf("Connection closed\n");
          closesocket(currentSocket);
      }

      return iResult;
  }

  return 0;
}

void ServerNetwork::sockReceive() {
  Packet packet;
  std::map<unsigned int, SOCKET>::iterator iter;

  for(iter = sessions.begin(); iter != sessions.end(); iter++)
  {
    int data_length = receiveData(iter->first, network_data);

    if (data_length <= 0) {
        //no data recieved
        continue;
    }


    int i = 0;
    while (i < static_cast<unsigned int>(data_length)){
      packet.deserialize(&(network_data[i]));
      i += sizeof(Packet);

      switch (packet.packet_type) {

          case INIT_CONNECTION:

              printf("server received init packet from client\n");
              break;

          case ACTION_EVENT:

              printf("server received action event packet from client\n");

              sendActionPackets();

              break;

          default:

              printf("Nothing to worry about\n");

              break;
        }
    }
  }
}

void ServerNetwork::sockSend(char * packets, int totalSize) {
  SOCKET currentSocket;
  std::map<unsigned int, SOCKET>::iterator iter;
  int iSendResult;

  for (iter = sessions.begin(); iter != sessions.end(); iter++)
  {
    currentSocket = iter->second;
    iSendResult = NetworkServices::sendMessage(currentSocket, packets, totalSize);

    if (iSendResult == SOCKET_ERROR)
    {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(currentSocket);
    }

  }
  printf("->Server sent time to current client\n");

}

// accept new connections
bool ServerNetwork::acceptNewClient(unsigned int & id){
  // if client waiting, accept the connection and save the socket
  printf("Server Accepting client-%d....",id);

  ClientSocket = accept(ListenSocket,nullptr,nullptr);

  if (ClientSocket != INVALID_SOCKET)
  {
    //disable nagle on the client's socket
    char value = 1;
    setsockopt( ClientSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof( value ) );

    // insert new client into session id table
    sessions.insert( std::pair<unsigned int, SOCKET>(id, ClientSocket) );

    printf("Succeed!!!! \n",id);

    return true;
  }

  printf("Failed T.T \n",id);
  return false;
}


//************************************************* PIPE ************************************************************
//
//void ServerNetwork::pipeInit() {
//    printf("Connecting to Admin inited server named pipe...");
//    pipe = CreateFile(
//            _T("\\\\.\\pipe\\my_pipe"),
//            GENERIC_WRITE, // only need read access
//            FILE_SHARE_READ | FILE_SHARE_WRITE,
//            NULL,
//            OPEN_EXISTING,
//            FILE_ATTRIBUTE_NORMAL,
//            NULL
//    );
//
//    if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
//        printf("Failed...Exiting\n");
//        exit(1);
//    }
//
//
//
//    printf("Complete!\n");
//
//    printf("Connecting to Admin named pipe...");
//    adminPipe = CreateFile(
//            _T("\\\\.\\pipe\\my_admin_pipe"),
//            GENERIC_WRITE, // only need read access
//            FILE_SHARE_READ | FILE_SHARE_WRITE,
//            NULL,
//            OPEN_EXISTING,
//            FILE_ATTRIBUTE_NORMAL,
//            NULL
//    );
//
//    if (adminPipe == NULL || adminPipe == INVALID_HANDLE_VALUE) {
//        printf("Failed...Exiting\n");
//
//        exit(1);
//    }
//
//    printf("Complete!\n");
//
//}
//
//void ServerNetwork::pipeReceive() {
//    printf("Waitting to connect to client........");
//
//    BOOL result = ConnectNamedPipe(pipe, NULL);
//    if (!result) {
//        printf("Connection failed...Retrying\n");
////        // look up error code here using GetLastError()
////        CloseHandle(pipe); // close the pipe
////        system("pause");
////        exit(1) ;
//    return;
//    }
//    printf("Client connected to server!!!\n");
//
//}
//
//void ServerNetwork::pipeSend(char *packets, int totalSize) {
//    printf("Sending data to client...");
//
//    // This call blocks until a client process reads all the data
//    DWORD numBytesWritten = 0;
//
//    iResult = WriteFile(
//            pipe, // handle to our outbound pipe
//            packets, // data to send
//            totalSize, // length of data to send (bytes)
//            &numBytesWritten, // will store actual amount of data sent
//            NULL // not using overlapped IO
//    );
//
//    if (iResult) {
//        printf("Number of bytes sent:%d...",numBytesWritten);
//    } else {
//        printf("Failed to send data...T.T...");
//        // look up error code here using GetLastError()
//    }
//
//
//    printf("Server done!\n");
//}
//
//void ServerNetwork::pipeSendToAdmin(char *packets, int totalSize) {
//    printf("Sending data to Admin...");
//
//    // This call blocks until a client process reads all the data
//    DWORD numBytesWritten = 0;
//
//    iResult = WriteFile(
//            adminPipe, // handle to our outbound pipe
//            packets, // data to send
//            totalSize, // length of data to send (bytes)
//            &numBytesWritten, // will store actual amount of data sent
//            NULL // not using overlapped IO
//    );
//
//    if (iResult) {
//        printf("Number of bytes sent:%d...",numBytesWritten);
//    } else {
//        printf("Failed to send data... T.T Retrying\n");
//        // look up error code here using GetLastError()
//    }
//
//}
//
//void ServerNetwork::pipeSignalActiveAdmin(){
//    Packet pack;
//    pack.packet_type = ACTION_EVENT;
//    pack.id = 0 ;
//
//    const unsigned int packet_size = sizeof (pack);
//    char packet_data[packet_size];
//
//    pack.serialize(packet_data);
//
//    iResult = false;
//    while (!iResult)
//        pipeSendToAdmin(packet_data,packet_size);
//}

