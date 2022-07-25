#include "client_network.h"

#include <tchar.h>



ClientNetwork::ClientNetwork(bool is_server, int type) {
  this->is_server = is_server;
  this->IPC_type = type ;
  if (!is_server){
      if (type == 0) {
          sockInit();
      } else {
          pipe_controller.init();
      }
  }

}


ClientNetwork::~ClientNetwork()
= default;

int ClientNetwork::receivePackets(char * recvbuf)
{
  if (is_server) return 0;

  if (IPC_type == 0) {
      sockReceive(recvbuf);
  } else {
      pipe_controller.receive(recvbuf);
  }


  return iResult;
}

void ClientNetwork::sendActionPackets() {
  if (is_server) return ;

  const unsigned int packet_size = sizeof(Packet);
  char packet_data[packet_size];

  Packet packet;
  packet.packet_type = ACTION_EVENT;

  packet.serialize(packet_data);


  //socket send
  if (IPC_type == 0) {
      sockSend(packet_data,packet_size);
  } else {
      pipe_controller.send(packet_data,packet_size);
  }
}

void ClientNetwork::update() {
  if (is_server) return ;

  memset(network_data, 0, sizeof network_data);
  resolveAdminData();

  //check on server
  if(is_server) return ;

  memset(network_data, 0, sizeof network_data);
  receivePackets(network_data);



  printf("Client-time report: %s \n",network_data);
}

 bool ClientNetwork::isServer() {
  return is_server;
}

void ClientNetwork::makeServer() {
  this->is_server = true ;
  CloseHandle(pipe_controller.pipe);
  CloseHandle(pipe_controller.admin_read_pipe);
  CloseHandle(pipe_controller.admin_write_pipe);

  this->server = new ServerNetwork(IPC_type,pipe_controller.admin_write_pipe,pipe_controller.admin_read_pipe);
}

void ClientNetwork::resolveAdminData(){

  pipe_controller.receiveFromAdmin(network_data);

//  Packet packet;
//  packet.deserialize(network_data);

  if (strcmp(network_data,"ACTION_EVENT") ==0){
    printf("Client receive server assignment\n");

    this->makeServer();
  }else {
    printf("Client received weird message:");
    printf(network_data);
    printf("\n");
  }

//  switch (packet.packet_type) {
//
//      case ACTION_EVENT:
//
//          printf("Client receive server assignment\n");
//
//          this->makeServer();
//
//          break;
//
//      default:
//
//          printf("Client received something not worth mentioning\n");
//
//          break;
//  }
}



//**************** SOCKET ****************

void ClientNetwork::sockInit() {

  printf("***************************************** CLIENT INIT ****************************************\n");
  // init as a client
  is_server = false;
  // create WSADATA object
  WSADATA wsaData;
  // socket
  ConnectSocket = INVALID_SOCKET;

  // holds address info for socket to connect to
  struct addrinfo *result = nullptr,
          *ptr = nullptr,
          hints;

  // Initialize Winsock
  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

  if (iResult != 0) {
      printf("WSAStartup failed with error: %d\n", iResult);
      exit(1);
  }
  printf("Init Winsock successfully!!!!\n");


  // set address info
  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;  //TCP connection!!!


  //resolve server address and port
  iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);

  if (iResult != 0) {
      printf("getaddrinfo failed with error: %d\n", iResult);
      WSACleanup();
      exit(1);
  }

  // Attempt to connect to an address until one succeeds
  for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {

      // Create a SOCKET for connecting to server
      printf("Creating connect socket for client....");
      ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                             ptr->ai_protocol);

      if (ConnectSocket == INVALID_SOCKET) {
          printf("socket failed with error: %ld\n", WSAGetLastError());
          WSACleanup();
          exit(1);
      }

      printf("Success!!!\n");

      // Connect to server.
      printf("Connecting to server....");
      iResult = connect(ConnectSocket, ptr->ai_addr, (int) ptr->ai_addrlen);

      if (iResult == SOCKET_ERROR) {
          closesocket(ConnectSocket);
          ConnectSocket = INVALID_SOCKET;
          printf("The server is down... did not connect\n");
      }


  }



  // no longer need address info for server
  freeaddrinfo(result);



  // if connection failed
  if (ConnectSocket == INVALID_SOCKET) {
      printf("Unable to connect to server!\n");
      //reconnect
      if (ConnectSocket == INVALID_SOCKET){
          printf("Setting client to be server\n");
          WSACleanup();

          this->makeServer(); // allocate
          return ;
      }

  }

  printf("Connected!!!!\n");

  // Set the mode of the socket to be nonblocking
  u_long iMode = 1;

  printf("Setting non blocking socket....");
  iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
  if (iResult == SOCKET_ERROR) {
      printf("ioctlsocket failed with error: %d\n", WSAGetLastError());
      closesocket(ConnectSocket);
      WSACleanup();
      exit(1);
  }

  printf("Success!!!!\n");

  //disable nagle
  char value = 1;
  setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, &value, sizeof(value));

  const unsigned int packet_size = sizeof(Packet);
  char packet_data[packet_size];

  Packet packet;
  packet.packet_type = INIT_CONNECTION;

  packet.serialize(packet_data);

  NetworkServices::sendMessage(ConnectSocket, packet_data, packet_size);
}

inline void ClientNetwork::sockSend(char *packet_data,unsigned int packet_size){

  NetworkServices::sendMessage(ConnectSocket, packet_data, packet_size);

}

inline void ClientNetwork::sockReceive(char * recvbuf) {
  iResult = NetworkServices::receiveMessage(ConnectSocket, recvbuf, MAX_PACKET_SIZE);

  if ( iResult == 0 )
  {
      printf("Connection closed\n");
      closesocket(ConnectSocket);
      WSACleanup();
      exit(1);
  }

}

