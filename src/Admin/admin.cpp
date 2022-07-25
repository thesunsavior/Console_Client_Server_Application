#include "admin.h"

#include <cstdio>
#include <iostream>


Admin::Admin() {
  client_id = 0 ;
  total_process=0;
  is_send = false;
}

std::string Admin::GetLastErrorAsString()
{
  //Get the error message ID, if any.
  DWORD errorMessageID = ::GetLastError();
  if(errorMessageID == 0) {
    return std::string(); //No error message has been recorded
  }

  LPSTR messageBuffer = nullptr;

  //Ask Win32 to give us the string version of that message ID.
  //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
  size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                               NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

  //Copy the error message into a std::string.
  std::string message(messageBuffer, size);

  //Free the Win32's string's buffer.
  LocalFree(messageBuffer);

  return message;
}

void Admin::connectWrite()  {
  printf("Waitting to connect to client to Admin write...");

  iResult = ConnectNamedPipe( this->admin_Write_Pipe, NULL);
  if (!iResult) {
      printf("Connection failed...error:");
      std::cout<<GetLastErrorAsString()<<std::endl;
//      printf("\n");
//        // look up error code here using GetLastError()
//        CloseHandle(pipe); // close the pipe
//        system("pause");
//      exit(1) ;
      return;
  }

  printf("Detected client connect to listen to Admin!\n");

}

void Admin::connectRead()  {
  printf("Waitting to connect to client to Admin read...");
  iResult = ConnectNamedPipe(this->admin_Read_Pipe, NULL);
  if (!iResult) {
    printf("Connection failed...error:");
    std::cout<<GetLastErrorAsString()<<std::endl;
//    printf("\n");
// look up error code here using GetLastError()
//        CloseHandle(pipe); // close the pipe
//        system("pause");
//        exit(1) ;
      return;
  }

  printf("Detected client connect to write to Admin!\n");
}

//establish a read pipe and write pipe for a new client
void Admin::init(){
  //set up admin write pipe
  printf("Creating admin write named pipe...");
  this->admin_Write_Pipe = CreateNamedPipe(
      _T("\\\\.\\pipe\\my_admin_write_pipe"), // name of the pipe
      PIPE_ACCESS_OUTBOUND, // Write only
      PIPE_TYPE_BYTE, // send data as a byte stream
      PIPE_UNLIMITED_INSTANCES , // only allow 1 instance of this pipe
      0, // no outbound buffer
      0, // no inbound buffer
      0, // use default wait time
      NULL // use default security attributes
  );

  if ( this->admin_Write_Pipe == NULL ||  this->admin_Write_Pipe == INVALID_HANDLE_VALUE) {
    printf("Failed....Exiting\n");
    exit(1);
  }

  printf("Admin Write pipe sucessfully created\n");

  //set up admin read pipe
  printf("Creating admin read named pipe...");
  admin_Read_Pipe = CreateNamedPipe(
      _T("\\\\.\\pipe\\my_admin_read_pipe"), // name of the pipe
      PIPE_ACCESS_INBOUND, // read only
      PIPE_TYPE_BYTE, // send data as a byte stream
      PIPE_UNLIMITED_INSTANCES , // only allow 1 instance of this pipe
      0, // no outbound buffer
      0, // no inbound buffer
      0, // use default wait time
      NULL // use default security attributes
  );

  if (admin_Read_Pipe == NULL || admin_Read_Pipe == INVALID_HANDLE_VALUE) {
    printf("Failed....Exiting\n");
    exit(1);
  }

  printf("Admin read pipe sucessfully created\n");
}

void Admin::receive(char * recvbuf) {
  printf("Admin Reading data from pipe...");
  // The read operation will block until there is data to read

  DWORD numBytesRead = 0;
  BOOL result = ReadFile(
          admin_Read_Pipe,
          recvbuf, // the data from the pipe will be put here
          256 * sizeof(char*), // number of bytes allocated
          &numBytesRead, // this will store number of bytes actually read
          NULL // not using overlapped IO
  );

  iResult  = result;

  if (result) {
      printf("Number of bytes Read:%d...",numBytesRead);
  } else {
      printf("Failed to read data T.T \n");
  }

  printf("Done!\n");

}

void Admin::send(char *packets, int totalSize) {
  printf("Sending data to client...");

  // This call blocks until a client process reads all the data
  DWORD numBytesWritten = 0;

  iResult = WriteFile(
      this->admin_Write_Pipe, // handle to our outbound pipe
          packets, // data to send
          totalSize, // length of data to send (bytes)
          &numBytesWritten, // will store actual amount of data sent
          NULL // not using overlapped IO
  );

  if (iResult) {
      printf("Number of bytes sent:%d...",numBytesWritten);
  } else {
      printf("Failed to send data...T.T \n");
      // look up error code here using GetLastError()
      return;
  }


  printf("Server done!\n");
}

void Admin::receiveFromClients(){

  iResult = true ;
  if (iResult){
      memset(network_data, 0, sizeof network_data);
      receive(network_data);

//      Packet packet;
//      packet.deserialize(networkData);

      if (!iResult){
          printf("Failed to receive \n");
//          break;
      }

      printf("ADMIN has receive packet\n");

      if (strcmp(network_data,"INIT_CONNECTION")==0){
        printf("ADMIN initializing CLIENT ID %d\n",client_id+1);
        total_process++;
        count_down = total_process;
      }else if (strcmp(network_data,"ACTION_EVENT")==0){
        printf("Active server confirmed\n");
      } else if(strcmp(network_data,"DEAD_SERVER")==0){
        printf("Dead Reported\n");
        count_down -- ;
        if (count_down <= 0 ){
          total_process--;
          count_down = total_process;
          printf("SERVER IS DEAD! ASSIGNING NEW SERVER\n");
          serverAssign();
        }
      }else {
        printf("weird message: ");
        printf(network_data);
        printf("\n");
      }
//      switch (networkData) {
//
//          case "INIT_CONNECTION": {
//              printf("ADMIN initializing CLIENT ID %d\n",client_id+1);
//
//              totalProcess++;
//              countDown = totalProcess;
//
//              break;
//          }
//
//          case ACTION_EVENT:{
//              printf("Active server confirmed\n");
//              break;
//          }
//
//          case DEAD_SERVER:{
//              countDown -- ;
//              if (countDown == 0 ){
//                  countDown = totalProcess;
//                  printf("SERVER IS DEAD! ASSIGNING NEW SERVER\n");
//                  serverAssign();
//                  break;
//              }
//          }
//
//
//          default:
//              printf("error in packet types\n");
//              break;
//      }
  }

}

void Admin::serverAssign(){
//  Packet serverPack;
//  serverPack.packet_type = ACTION_EVENT;
//  serverPack.id = 0;

//  const unsigned int packet_size = sizeof(serverPack);
//  char *packet_data = "ACTION_EVENT";
//  const unsigned int packet_size = strlen(packet_data);
////  serverPack.serialize(packet_data);
//
//  send(packet_data,packet_size);
  is_send = true;
  send_content = "ACTION_EVENT";
}

void Admin::update() {
//  connectWrite();
//  connectRead();

  if(is_send){
    printf("This turn is for sending message\n");
    send(send_content, strlen(send_content));
    while(!iResult)
      send(send_content, strlen(send_content));
    is_send = false ;
  }else {
    printf("This turn is for reading message\n");
    receiveFromClients();
  }
}