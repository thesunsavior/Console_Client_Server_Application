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
    if (admin.is_send) {
      admin.connectWrite();
      if (!admin.iResult && GetLastError() != ERROR_PIPE_CONNECTED) {
        continue;
      }
    } else {
      admin.connectRead();
      if (!admin.iResult && GetLastError() != ERROR_PIPE_CONNECTED) {
        continue;
      }
    }

    admin.update();
  }

  return 0;
}
