#ifndef Excution_h
#define Excution_h
#include "Arduino.h"
#include "config.h"
class Execution {
private:
public:
  Execution();
  void begin();
  void drainOut(int poolID);
  void supplyIn(int poolID);
  void xdrainOut(int poolID);
  void xsupplyIn(int poolID);
  void autoRun(int poolID);
};
#endif