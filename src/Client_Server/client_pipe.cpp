#include "client_pipe.h"

#include <tchar.h>
#include <winbase.h>

#include <iostream>

HANDLE ClientPipe::pipe = INVALID_HANDLE_VALUE;
HANDLE ClientPipe::admin_read_pipe = INVALID_HANDLE_VALUE;
HANDLE ClientPipe::admin_write_pipe = INVALID_HANDLE_VALUE;

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

void ClientPipe::init() {

  sendPackageToAdmin("INIT_CONNECTION");

  std::cout << "Connecting to server pipe...";

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
    std::cout << "Server Connection failed...error:" << GetLastErrorAsString() << std::endl;
    std::cout << "Reporting to admin" << std::endl;

    // look up error code here using GetLastError()

    sendPackageToAdmin("DEAD_SERVER");

    sleep(5);

    return;
  }

  std::cout << "Complete!" << std::endl;
}

bool ClientPipe::connectToAdmin() {
  return connectToAdminWrite() && connectToAdminRead();
}

bool ClientPipe::connectToAdminRead() {
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
    std::cout << "Connection failed...error:" << GetLastErrorAsString() << std::endl;
    return false;
  }

  std::cout << "Complete" << std::endl;
  return true;
}

bool ClientPipe::connectToAdminWrite() {
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
    std::cout << "Connection failed...error:" << GetLastErrorAsString() << std::endl;
    return false;
  }

  std::cout << "Complete" << std::endl;

  return true;
}

void ClientPipe::sendToAdmin(char *packets, int32_t totalSize) {

  std::cout << "Connecting to admin..." << std::endl;
  while (!connectToAdminWrite()) { sleep(rand() % 3); };
  while (!connectToAdminRead()) { sleep(rand() % 3); };

  std::cout << "Sending data to admin..." << std::endl;

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
    std::cout << "Number of bytes sent:" << numBytesWritten << std::endl;
  } else {
    std::cout << "Failed to send...error:" << GetLastErrorAsString() << std::endl;
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
  std::cout << "Connecting to admin..." << std::endl;
  if (!connectToAdminWrite()) {
    std::cout << "Connection failed...error:" << GetLastErrorAsString() << std::endl;
    return;
  }
  while (!connectToAdminRead())
    ;

  DWORD total_available_bytes;

  PeekNamedPipe(admin_write_pipe,
                0,
                0,
                reinterpret_cast<LPDWORD>(127 * sizeof(char *)),
                0,
                &total_available_bytes);

  if (total_available_bytes <= 0) {
    std::cout << "Nothing in admin pipe" << std::endl;
    return;
  }

  std::cout << "Client Reading data from Admin pipe...";
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
    std::cout << "Number of bytes Read:" << numBytesRead << " ";
  } else {
    std::cout << "Failed to read data...error:" << GetLastErrorAsString() << std::endl;
    return;
  }

  std::cout << "Done!" << std::endl;
}

void ClientPipe::receive(char *recvbuf) {
  std::cout << "Client Reading data from pipe...";
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
    std::cout << "Number of bytes Read:" << numBytesRead << std::endl;
  } else {
    std::cout << "Failed to read data T.T , error:" << GetLastErrorAsString() << std::endl;

    if (GetLastError() == ERROR_BROKEN_PIPE) {
      std::cout << "Server died, reporting. Wait for assignment" << std::endl;
      // look up error code here using GetLastError()

      sendPackageToAdmin("DEAD_SERVER");

      sleep(5);

      return;
    }
  }

  std::cout << "Done!" << std::endl;
}