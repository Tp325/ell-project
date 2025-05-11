#ifndef Storage_h
#define Storage_h
#include "Arduino.h"
#include <EEPROM.h>
#include "config.h"
class Storage {
private:
  float buffReadData;
  int IDOfPool;
  bool hasData = false;
  bool needCommit = false;
  float maxValueInEEPROM;
  float midValueInEEPROM;
  float minValueInEEPROM;
public:
  Storage();
  void begin();
  void saveToEEPROM();
  float readFromEEPROM(int adrress);
};
#endif