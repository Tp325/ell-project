#ifndef Display_h
#define Display_h
#include "Arduino.h"
#include <U8g2lib.h>
#include "config.h"
class Display {
private:
  U8G2_ST7920_128X64_F_SW_SPI *u8g2;
  int enableLed;
public:
  Display(int clock = 18, int data = 23, int CS = 5, int reset = 27, int enableLed = 12);
  void begin();
  void sleepDisplay();
  void onDisplay();
  void screenOn();
  void homePage(Pool &A);    // page 1..n
  void settingPage(Pool A);  //page 0
};
#endif