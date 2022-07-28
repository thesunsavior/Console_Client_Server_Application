#include "client_pipe.h"

#include <tchar.h>
#include <winbase.h>

#include <iostream>

#include "Logger.h"

HANDLE ClientPipe::pipe = INVALID_HANDLE_VALUE;
HANDLE ClientPipe::admin_read_pipe = INVALID_HANDLE_VALUE;
HANDLE ClientPipe::admin_write_pipe = INVALID_HANDLE_VALUE;
bool ClientPipe::reported = false;

std::string GetLastErrorAsString() {
  //Get the error message ID, if any.
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0) {
    return std::string();//No error message has been recorded
  }

  LPSTR messageBuffer = nullptr;

  //Ask Win32 to give us the string version of that message ID.
  //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
  size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                               NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &messageBuffer, 0, NULL);

  //Copy the error message into a std::string.
  std::string message(messageBuffer, size);

  //Free the Win32's string's buffer.
  LocalFree(messageBuffer);

  return message;
}

void ClientPipe::connect() {
  LOG(DEBUG) << "Connecting to server pipe...";

  // Open the named pipe
  // Most of these parameters aren't very relevant for pipes.
  pipe = CreateFile(
      _T("\\\\.\\pipe\\my_pipe"),
      GENERIC_READ,// only need read access
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

  if (pipe == nullptr || pipe == INVALID_HANDLE_VALUE) {
    LOG(ERRORS) << "Server Connection failed...error:" << GetLastErrorAsString();
    return;
  }

  LOG(DEBUG) << "Reconnected!";
}

void ClientPipe::init() {

  sendPackageToAdmin("INIT_CONNECTION");

  LOG() << "Connecting to server pipe...";

  // Open the named pipe
  // Most of these parameters aren't very relevant for pipes.
  pipe = CreateFile(
      _T("\\\\.\\pipe\\my_pipe"),
      GENERIC_READ,// only need read access
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      nullptr,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      nullptr);

  if (pipe == nullptr || pipe == INVALID_HANDLE_VALUE) {
    LOG(ERRORS) << "Server Connection failed...error:" << GetLastErrorAsString();
    LOG(ERRORS) << "Reporting to admin";

    // look up error code here using GetLastError()

    sendPackageToAdmin("DEAD_SERVER");

    sleep(5);

    return;
  }

  LOG() << "Connection process completed!";
}

bool ClientPipe::connectToAdmin() {
  return connectToAdminWrite() && connectToAdminRead();
}

bool ClientPipe::connectToAdminRead() {
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
    LOG(ERRORS) << "Connection failed...error:" << GetLastErrorAsString();
    return false;
  }

  LOG() << "Connection completed!";
  return true;
}

bool ClientPipe::connectToAdminWrite() {
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
    LOG(ERRORS) << "Connection failed...error:" << GetLastErrorAsString();
    return false;
  }

  LOG() << "Connection completed!";

  return true;
}

void ClientPipe::sendToAdmin(char *packets, int32_t totalSize) {

  LOG() << "Connecting to admin...";
  //  while (!connectToAdminWrite()) { sleep(rand() % 3); };
  while (!connectToAdminRead()) { sleep(rand() % 3); };

  LOG() << "Sending data to admin...";

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
    LOG(ERRORS) << "Number of bytes sent:" << numBytesWritten;
  } else {
    LOG(ERRORS) << "Failed to send...error:" << GetLastErrorAsString();
  }
}

void ClientPipe::sendPackageToAdmin(char *type) {

  uint32_t packet_size = strlen(type);

  iResult = false;
  while (!iResult) {
    sendToAdmin(type, packet_size);
  }
}

void ClientPipe::receiveFromAdmin(char *recvbuf) {
  LOG() << "Connecting to admin...";
  while (!connectToAdminWrite()) {
    LOG(ERRORS) << "Connection failed...error:" << GetLastErrorAsString();
    if (GetLastError() == ERROR_INVALID_HANDLE || GetLastError() == ERROR_FILE_NOT_FOUND)
      return;
    //    return;
  }

  LOG() << "Client Reading data from Admin pipe...";

  // The read operation will block until there is data to read
  DWORD numBytesRead = 0;
  BOOL result = ReadFile(
      admin_write_pipe,
      recvbuf,             // the data from the pipe will be put here
      127 * sizeof(char *),// number of bytes allocated
      &numBytesRead,       // this will store number of bytes actually read
      nullptr              // not using overlapped IO
  );

  iResult = result;

  if (result) {
    LOG() << "Number of bytes Read:" << numBytesRead << " ";
  } else {
    LOG(ERRORS) << "Failed to read data...error:" << GetLastErrorAsString();
    return;
  }

  LOG() << "Done!";
}

void ClientPipe::receive(char *recvbuf) {

  if (reported) {
    CloseHandle(pipe);
    connect();
  }

  LOG() << "Client Reading data from pipe...";
  // The read operation will block until there is data to read

  DWORD numBytesRead = 0;
  BOOL result = ReadFile(
      pipe,
      recvbuf,             // the data from the pipe will be put here
      127 * sizeof(char *),// number of bytes allocated
      &numBytesRead,       // this will store number of bytes actually read
      nullptr              // not using overlapped IO
  );

  iResult = result;

  if (result) {
    reported = false;
    LOG() << "Number of bytes Read:" << numBytesRead;
  } else {
    LOG(ERRORS) << "Failed to read data T.T , error:" << GetLastErrorAsString();

    if (GetLastError() == ERROR_BROKEN_PIPE && !reported) {
      LOG(INFO) << "Server died, reporting. Wait for assignment";
      // look up error code here using GetLastError()

      sendPackageToAdmin("DEAD_SERVER");
      reported = true;

      sleep(rand() % 5);

      return;
    }
  }

  LOG() << "Done!";
}