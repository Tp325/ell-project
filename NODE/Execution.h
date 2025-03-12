#ifndef Excution_h
#define Excution_h
#include "Arduino.h"
#include "config.h"
#include "Sensor.h"
class Execution {
private:
  float bufferDistance[10];
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