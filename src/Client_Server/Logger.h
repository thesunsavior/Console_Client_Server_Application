#ifndef CONSOLE_CLIENT_SERVER_APPLICATION_MASTER_LOGGER_H
#define CONSOLE_CLIENT_SERVER_APPLICATION_MASTER_LOGGER_H
#include <iostream>
#include <sstream>

using namespace std;

enum typelog {
  DEBUG,
  INFO,
  ERRORS,
};

class LOG {
  public:
  LOG() { msglevel = DEBUG; }
  LOG(typelog type) {
    msglevel = type;

    operator<<("[" + getLabel(type) + "]");
  }

  ~LOG() {
    if (opened) {
      cerr << endl;
    }
    opened = false;
  }

  template<class T>
  LOG &operator<<(const T &msg) {

    cerr << msg;
    opened = true;

    return *this;
  }

  private:
  bool opened = false;
  typelog msglevel = DEBUG;
  inline string getLabel(typelog type) {
    string label;
    switch (type) {
      case DEBUG:
        label = "DEBUG:";
        break;
      case INFO:
        label = "SYSTEM ANNOUNCEMENT:";
        break;
      case ERRORS:
        label = "ERROR:";
        break;
    }
    return label;
  }
};

#endif//CONSOLE_CLIENT_SERVER_APPLICATION_MASTER_LOGGER_H
