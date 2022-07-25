#include "server_pipe.h"

#include <iostream>

#include <winbase.h>
#include <tchar.h>



void ServerPipe::init(HANDLE cadmin_write_pipe, HANDLE cadmin_read_pipe) {
  printf("Creating server pipe...");
  pipe = CreateNamedPipe(
      _T("\\\\.\\pipe\\my_pipe"), // name of the pipe
      PIPE_ACCESS_OUTBOUND, // 1-way pipe -- send only
      PIPE_TYPE_BYTE, // send data as a byte stream
      PIPE_UNLIMITED_INSTANCES, // only allow 1 instance of this pipe
      0, // no outbound buffer
      0, // no inbound buffer
      0, // use default wait time
      NULL // use default security attributes
  );

  if (pipe == nullptr || pipe == INVALID_HANDLE_VALUE) {
      printf("Failed...Exiting\n");
      exit(1);
  }



  printf("Complete!\n");

//  printf("Connecting to Admin write named pipe...");
////  admin_write_pipe = CreateFile(
////      _T("\\\\.\\pipe\\my_admin_write_pipe"),
////      GENERIC_READ, // only need read access
////      FILE_SHARE_READ,
////      nullptr,
////      OPEN_EXISTING,
////      FILE_ATTRIBUTE_NORMAL,
////      nullptr
////  );
//  admin_write_pipe = cadmin_write_pipe;
//
//  if (admin_write_pipe == nullptr || admin_write_pipe == INVALID_HANDLE_VALUE) {
//      printf("Failed...Exiting\n");
//
//      exit(1);
//  }
//
//  printf("Complete!\n");
//
//  printf("Connecting to Admin read named pipe...");
////  admin_read_pipe = CreateFile(
////      _T("\\\\.\\pipe\\my_admin_read_pipe"),
////      GENERIC_WRITE, // only need read access
////      FILE_SHARE_WRITE,
////      nullptr,
////      OPEN_EXISTING,
////      FILE_ATTRIBUTE_NORMAL,
////      nullptr
////  );
//
//  admin_read_pipe = cadmin_read_pipe;
//
//  if (admin_read_pipe == nullptr || admin_read_pipe == INVALID_HANDLE_VALUE) {
//      printf("Failed...Exiting\n");
//
//      exit(1);
//  }
//
//  printf("Complete!\n");
}

void ServerPipe::send(char *packets, int totalSize) {
  printf("Sending data to client...");

  // This call blocks until a client process reads all the data
  DWORD numBytesWritten = 0;

  iResult = WriteFile(
          pipe, // handle to our outbound pipe
          packets, // data to send
          totalSize, // length of data to send (bytes)
          &numBytesWritten, // will store actual amount of data sent
          nullptr // not using overlapped IO
  );

  if (iResult) {
      printf("Number of bytes sent:%d...",numBytesWritten);
  } else {
      printf("Failed to send data...T.T...");
      // look up error code here using GetLastError()
  }


  printf("Server done!\n");
}

void ServerPipe::connect(){
  printf("Waitting to connect to client........");

  BOOL result = ConnectNamedPipe(pipe, nullptr);
  if (!result) {
      printf("Connection failed...Retrying\n");
//        // look up error code here using GetLastError()
//        CloseHandle(pipe); // close the pipe
//        system("pause");
//        exit(1) ;
      return;
  }
  printf("Client connected to server!!!\n");
}

boolean ServerPipe::connectToAdmin(){
  printf("Connecting to Admin write named pipe...");
  admin_write_pipe = CreateFile(
      _T("\\\\.\\pipe\\my_admin_write_pipe"),
      GENERIC_READ, // only need read access
      FILE_SHARE_READ,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr
  );

  if (admin_write_pipe == nullptr || admin_write_pipe == INVALID_HANDLE_VALUE) {
    printf("Failed...Exiting\n");

    return false;
  }

  printf("Complete!\n");


  printf("Connecting to Admin read named pipe...");
  admin_read_pipe = CreateFile(
      _T("\\\\.\\pipe\\my_admin_read_pipe"),
      GENERIC_WRITE, // only need read access
      FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr
  );

  if (admin_read_pipe == nullptr || admin_read_pipe == INVALID_HANDLE_VALUE) {
    printf("Failed...retry later\n");

    return false;
  }

  printf("Complete!\n");


  return true ;
}

void ServerPipe::sendToAdmin(char *packets, int totalSize) {
  printf("Connecting to admin...\n");
  while (!connectToAdmin()){sleep(rand()%3);};
  printf("Sending data to Admin...");

  // This call blocks until a client process reads all the data
  DWORD numBytesWritten = 0;

  iResult = WriteFile(
          admin_read_pipe, // handle to our outbound pipe
          packets, // data to send
          totalSize, // length of data to send (bytes)
          &numBytesWritten, // will store actual amount of data sent
          nullptr // not using overlapped IO
  );

  if (iResult) {
      printf("Number of bytes sent:%d...",numBytesWritten);
  } else {
      printf("Failed to send data... T.T Retrying\n");
      // look up error code here using GetLastError()
  }

}

void ServerPipe::signalActiveAdmin() {
//  Packet pack;
//  pack.packet_type = ACTION_EVENT;
//  pack.id = 0 ;
//
//  const unsigned int packet_size = sizeof (pack);
  char *packet_data = "ACTION_EVENT";
  const unsigned int packet_size = strlen(packet_data);
//  pack.serialize(packet_data);

  iResult = false;
  while (!iResult) {
      sendToAdmin(packet_data,packet_size);
  }
}
