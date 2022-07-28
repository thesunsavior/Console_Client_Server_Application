#include <process.h>
#include <iostream>

#include "admin.h"

Admin admin;

int main() {
  freopen( "admin_error.txt", "w", stderr );

  std::cerr<<"---Admin activity---"<<std::endl;

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
