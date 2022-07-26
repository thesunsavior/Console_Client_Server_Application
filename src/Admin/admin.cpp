#include "admin.h"

#include <cstdio>
#include <iostream>

std::string GetLastErrorAsString()
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

Admin::Admin() {
  client_id = 0 ;
  total_process=0;
  is_send = false;
}



void Admin::connectWrite()  {
  std::cout<<"Waitting to connect to client to Admin write...";

  iResult = ConnectNamedPipe( this->admin_Write_Pipe, nullptr);
  if (!iResult) {
      std::cout<<"Connection failed...error:"<<GetLastErrorAsString()<<std::endl;
      return;
  }

  std::cout<<"Detected client connect to listen to Admin!"<<std::endl;

}

void Admin::connectRead()  {
  std::cout<<"Waitting to connect to client to Admin read...";
  iResult = ConnectNamedPipe(this->admin_Read_Pipe, nullptr);
  if (!iResult) {
    std::cout<<"Connection failed...error:"<<GetLastErrorAsString()<<std::endl;
      return;
  }
  std::cout<<"Detected client connect to write to Admin!"<<std::endl;
}

//establish a read pipe and write pipe for a new client
void Admin::init(){
  //set up admin write pipe
  std::cout<<"Creating admin write named pipe...";
  this->admin_Write_Pipe = CreateNamedPipe(
      _T("\\\\.\\pipe\\my_admin_write_pipe"), // name of the pipe
      PIPE_ACCESS_OUTBOUND, // Write only
      PIPE_TYPE_BYTE, // send data as a byte stream
      PIPE_UNLIMITED_INSTANCES , // only allow 1 instance of this pipe
      0, // no outbound buffer
      0, // no inbound buffer
      0, // use default wait time
      nullptr // use default security attributes
  );

  if ( this->admin_Write_Pipe == nullptr ||  this->admin_Write_Pipe == INVALID_HANDLE_VALUE) {
    std::cout<<" failed...error:"<<GetLastErrorAsString()<<std::endl;
    std::exit(1);
  }

  std::cout<<"Admin Write pipe sucessfully created"<<std::endl;

  //set up admin read pipe
  std::cout<<"Creating admin read named pipe...";
  admin_Read_Pipe = CreateNamedPipe(
      _T("\\\\.\\pipe\\my_admin_read_pipe"), // name of the pipe
      PIPE_ACCESS_INBOUND, // read only
      PIPE_TYPE_BYTE, // send data as a byte stream
      PIPE_UNLIMITED_INSTANCES , // only allow 1 instance of this pipe
      0, // no outbound buffer
      0, // no inbound buffer
      0, // use default wait time
      nullptr // use default security attributes
  );

  if (admin_Read_Pipe == nullptr || admin_Read_Pipe == INVALID_HANDLE_VALUE) {
    std::cout<<" failed...error:"<<GetLastErrorAsString()<<std::endl;
    exit(1);
  }
  std::cout<<"Admin read pipe sucessfully created"<<std::endl;
}

void Admin::receive(char * recvbuf) {
  std::cout<<"Admin Reading data from pipe...";
  // The read operation will block until there is data to read

  DWORD numBytesRead = 0;
  BOOL result = ReadFile(
          admin_Read_Pipe,
          recvbuf, // the data from the pipe will be put here
          256 * sizeof(char*), // number of bytes allocated
          &numBytesRead, // this will store number of bytes actually read
      nullptr // not using overlapped IO
  );

  iResult  = result;

  if (result) {
    std::cout<<"Number of bytes Read:..."<<numBytesRead<<std::endl;

  } else {
    std::cout<<"failed...error:"<<GetLastErrorAsString()<<std::endl;
  }

  std::cout<<"Done"<<std::endl;
}

void Admin::send(char *packets, int totalSize) {
  std::cout<<"Sending data to client...";

  // This call blocks until a client process reads all the data
  DWORD numBytesWritten = 0;

  iResult = WriteFile(
      this->admin_Write_Pipe, // handle to our outbound pipe
          packets, // data to send
          totalSize, // length of data to send (bytes)
          &numBytesWritten, // will store actual amount of data sent
          nullptr // not using overlapped IO
  );

  if (iResult) {
    std::cout<<"Number of bytes sent:..."<<numBytesWritten<<std::endl;
  } else {
    std::cout<<"failed...error:"<<GetLastErrorAsString()<<std::endl;
      return;
  }
  std::cout<<"Server done!"<<std::endl;
}

void Admin::receiveFromClients(){
  iResult = false ;
  while (!iResult){
      memset(network_data, 0, sizeof network_data);
      receive(network_data);

      if (!iResult){
          printf("Admin restart \n");
          continue ;
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
        std::cout<<"weird message: "<<network_data<<std::endl;
      }
  }
}

void Admin::serverAssign(){
  is_send = true;
  send_content = "ACTION_EVENT";
}

void Admin::update() {
//  connectWrite();
//  connectRead();

  if(is_send){
    std::cout<<"This turn is for sending message"<<std::endl;
    send(send_content, strlen(send_content));
    while(!iResult)
      send(send_content, strlen(send_content));
    is_send = false ;
  }else {
    std::cout<<"This turn is for reading message"<<std::endl;
    receiveFromClients();
  }
}

