//
// Created by trungpq on 7/22/2022.
//
#include <process.h>

#include "admin.h"

Admin admin;

int main() {
  HANDLE h_thread = nullptr;
  for (;;) {
    admin.init();
    admin.connectWrite();
    if (!admin.iResult && GetLastError() != ERROR_PIPE_CONNECTED) {
      continue;
    }
    admin.connectRead();
    if (!admin.iResult && GetLastError() != ERROR_PIPE_CONNECTED) {
      continue;
    }

    admin.update();
  }

  return 0;
}
