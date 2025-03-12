#ifndef Display_h
#define Display_h
#include "Arduino.h"
#include "config.h"
class Display {
private:
  void write_data(uint8_t data);
  void write_command(uint8_t command);
  void GLCD_TextGoTo(unsigned char x, unsigned char y);
  void cleardisplay();
  void cleartext();
  void display_bitmap(const uint8_t *display);
  void initialize_display();
  void GLCD_SetAddressPointer(unsigned int address);
  void GLCD_WriteDisplayData(unsigned char x);
  void GLCD_ClearCG(void);
  void GLCD_ClearGraphic(void);
  void GLCD_WriteString(String string);
  void GLCD_WriteChar(char charCode);
public:
  Display();
  void begin();
  void screenOn();
  void homePage(Pool A);     // page 1..n
  void settingPage(Pool A);  //page 0
};
class Button {
private:
public:
  Button();
  void begin();
  void checkState();
};
#endif