#include "server_pipe.h"

#include <tchar.h>
#include <winbase.h>

#include <chrono>
#include <iostream>

#include "Logger.h"
int32_t ServerPipe::iResult = true;
HANDLE ServerPipe::pipe = INVALID_HANDLE_VALUE;
HANDLE ServerPipe::admin_write_pipe = INVALID_HANDLE_VALUE;
HANDLE ServerPipe::admin_read_pipe = INVALID_HANDLE_VALUE;

std::string GetLastErrorAsString();

void ServerPipe::init() {
  LOG() << "Creating server pipe...";
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
    LOG(ERRORS) << "Failed...error:" << GetLastErrorAsString();
    std::exit(1);
  }


  LOG() << "Complete!";
}

void ServerPipe::send(char *packets, int32_t totalSize, HANDLE myPipe) {
  LOG() << "Sending data to client...";
  std::cout << "Sending data to client...";


  // This call blocks until a client process reads all the data
  DWORD numBytesWritten = 0;

  iResult = WriteFile(
      myPipe,          // handle to our outbound pipe
      packets,         // data to send
      totalSize,       // length of data to send (bytes)
      &numBytesWritten,// will store actual amount of data sent
      nullptr          // not using overlapped IO
  );

  if (iResult) {
    LOG() << "Number of bytes sent:" << numBytesWritten << " ";
  } else {
    LOG(ERRORS) << "Failed...error:" << GetLastErrorAsString();
  }

  LOG() << "Server done!";
  std::cout << "Server done!" << std::endl;
}

bool ServerPipe::connect() {
  LOG() << "Waitting to connect to client....";

  iResult = ConnectNamedPipe(pipe, nullptr);
  if (!iResult) {
    LOG(ERRORS) << "Failed...error:" << GetLastErrorAsString();
    return false;
  }
  LOG() << "Client connected to server!!!";
  return true;
}

boolean ServerPipe::connectToAdmin() {
  LOG() << "Connecting to Admin write named pipe...";
  admin_write_pipe = CreateFile(
      _T("\\\\.\\pipe\\my_admin_write_pipe"),
      GENERIC_READ,// only need read access
      FILE_SHARE_READ,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

  if (admin_write_pipe == nullptr || admin_write_pipe == INVALID_HANDLE_VALUE) {
    LOG(ERRORS) << "Failed...error:" << GetLastErrorAsString();
    return false;
  }

  LOG() << "Complete!";

  LOG() << "Connecting to Admin read named pipe...";
  admin_read_pipe = CreateFile(
      _T("\\\\.\\pipe\\my_admin_read_pipe"),
      GENERIC_WRITE,// only need read access
      FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

  if (admin_read_pipe == nullptr || admin_read_pipe == INVALID_HANDLE_VALUE) {
    LOG(ERRORS) << "Failed...error:" << GetLastErrorAsString();

    return false;
  }

  LOG() << "Complete!";


  return true;
}

void ServerPipe::sendToAdmin(char *packets, int totalSize) {
  LOG() << "Connecting to admin...";

  while (!connectToAdmin()) { sleep(rand() % 3); };
  LOG() << "Sending data to Admin...";

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
    LOG() << "Number of bytes sent:" << numBytesWritten;
  } else {
    LOG(ERRORS) << "Failed...error:" << GetLastErrorAsString();
  }
}

void ServerPipe::signalActiveAdmin() {
  char *packet_data = "ACTION_EVENT";
  const uint32_t packet_size = strlen(packet_data);

  iResult = false;
  while (!iResult) {
    sendToAdmin(packet_data, packet_size);
  }
}
