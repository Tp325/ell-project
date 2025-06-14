#ifndef Button_h
#define Button_h
#include "Arduino.h"
#include "config.h"
class Button {
private:
public:
  Button();
  void begin();
  void checkState();
};
#endif