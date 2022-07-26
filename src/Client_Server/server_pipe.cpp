#include "server_pipe.h"

#include <tchar.h>
#include <winbase.h>

#include <iostream>

std::string GetLastErrorAsString();

void ServerPipe::init(HANDLE cadmin_write_pipe, HANDLE cadmin_read_pipe) {
  std::cout << "Creating server pipe...";
  pipe = CreateNamedPipe(
      _T("\\\\.\\pipe\\my_pipe"),// name of the pipe
      PIPE_ACCESS_OUTBOUND,      // 1-way pipe -- send only
      PIPE_TYPE_BYTE,            // send data as a byte stream
      PIPE_UNLIMITED_INSTANCES,  // only allow 1 instance of this pipe
      0,                         // no outbound buffer
      0,                         // no inbound buffer
      0,                         // use default wait time
      nullptr                    // use default security attributes
  );

  if (pipe == nullptr || pipe == INVALID_HANDLE_VALUE) {
    std::cout << "Failed...error:" << GetLastErrorAsString() << std::endl;
    std::exit(1);
  }


  std::cout << "Complete!" << std::endl;
}

void ServerPipe::send(char *packets, int totalSize) {
  std::cout << "Sending data to client...";


  // This call blocks until a client process reads all the data
  DWORD numBytesWritten = 0;

  iResult = WriteFile(
      pipe,            // handle to our outbound pipe
      packets,         // data to send
      totalSize,       // length of data to send (bytes)
      &numBytesWritten,// will store actual amount of data sent
      nullptr          // not using overlapped IO
  );

  if (iResult) {
    std::cout << "Number of bytes sent:..." << numBytesWritten;
  } else {
    std::cout << "Failed...error:" << GetLastErrorAsString() << std::endl;
  }

  std::cout << "Server done!" << std::endl;
}

void ServerPipe::connect() {
  std::cout << "Waitting to connect to client....";

  BOOL result = ConnectNamedPipe(pipe, nullptr);
  if (!result) {
    std::cout << "Failed...error:" << GetLastErrorAsString() << std::endl;
    return;
  }
  std::cout << "Client connected to server!!!" << std::endl;
}

boolean ServerPipe::connectToAdmin() {
  std::cout << "Connecting to Admin write named pipe...";
  admin_write_pipe = CreateFile(
      _T("\\\\.\\pipe\\my_admin_write_pipe"),
      GENERIC_READ,// only need read access
      FILE_SHARE_READ,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

  if (admin_write_pipe == nullptr || admin_write_pipe == INVALID_HANDLE_VALUE) {
    std::cout << "Failed...error:" << GetLastErrorAsString() << std::endl;
    return false;
  }

  printf("Complete!\n");

  std::cout << "Connecting to Admin read named pipe...";
  admin_read_pipe = CreateFile(
      _T("\\\\.\\pipe\\my_admin_read_pipe"),
      GENERIC_WRITE,// only need read access
      FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

  if (admin_read_pipe == nullptr || admin_read_pipe == INVALID_HANDLE_VALUE) {
    std::cout << "Failed...error:" << GetLastErrorAsString() << std::endl;

    return false;
  }

  std::cout << "Complete!" << std::endl;


  return true;
}

void ServerPipe::sendToAdmin(char *packets, int totalSize) {
  std::cout << "Connecting to admin..." << std::endl;

  while (!connectToAdmin()) { sleep(rand() % 3); };
  std::cout << "Sending data to Admin..." << std::endl;

  // This call blocks until a client process reads all the data
  DWORD numBytesWritten = 0;

  iResult = WriteFile(
      admin_read_pipe, // handle to our outbound pipe
      packets,         // data to send
      totalSize,       // length of data to send (bytes)
      &numBytesWritten,// will store actual amount of data sent
      nullptr          // not using overlapped IO
  );

  if (iResult) {
    std::cout << "Number of bytes sent:" << numBytesWritten;
  } else {
    std::cout << "Failed...error:" << GetLastErrorAsString() << std::endl;
    // look up error code here using GetLastError()
  }
}

void ServerPipe::signalActiveAdmin() {
  char *packet_data = "ACTION_EVENT";
  const unsigned int packet_size = strlen(packet_data);

  iResult = false;
  while (!iResult) {
    sendToAdmin(packet_data, packet_size);
  }
}
