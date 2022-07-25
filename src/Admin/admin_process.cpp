//
// Created by trungpq on 7/22/2022.
//
#include "admin.h"

#include <process.h>

Admin admin;

void update(){
  admin.update();
}

int main(){
  HANDLE h_thread = nullptr;
  for(;;) {
    admin.init();
    admin.connectWrite();
    if (!admin.iResult && GetLastError() != ERROR_PIPE_CONNECTED) {
//      sleep (1);
      continue;
    }
    admin.connectRead();
    if (!admin.iResult && GetLastError() != ERROR_PIPE_CONNECTED){
//      sleep(1);
      continue;}

    update();
//    admin.update();
  }

  return 0;
}

