#include "Display.h"
Display::Display() {
}
void Display::begin() {
  pinMode(CHIP_ENABLE_PIN, OUTPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(DATA_COMMAND_PIN, OUTPUT);
  pinMode(WRITE_PIN, OUTPUT);
  pinMode(READ_PIN, OUTPUT);
  pinMode(LED_LCD, OUTPUT);
  digitalWrite(LED_LCD, HIGH);
  for (int pin = 0; pin <= 7; pin++) {
    pinMode(gpioPins[pin], OUTPUT);
  }
  digitalWrite(CHIP_ENABLE_PIN, HIGH);
  digitalWrite(RESET_PIN, HIGH);
  digitalWrite(DATA_COMMAND_PIN, HIGH);
  digitalWrite(WRITE_PIN, HIGH);
  digitalWrite(READ_PIN, HIGH);
  for (int pin = 0; pin <= 7; pin++) {
    digitalWrite(gpioPins[pin], HIGH);  // Tắt chân GPIO
  }
  SET_RD;
  SET_WR;
  SET_CS;
  // Initialize the display
  initialize_display();
  cleartext();
  cleardisplay();
  display_bitmap(logo);
  delay(2000);
  cleartext();
  cleardisplay();
  write_command(0x9C);
  cleartext();
  cleardisplay();  // display mode - graphic on, text on
}
void Display::write_command(uint8_t command) {
  // Select the LCD's command register
  SET_DC;
  // Select the LCD controller
  CLR_CS;
  // Send the data via parallel
  for (int bit = 0; bit <= 7; bit++) {
    digitalWrite(gpioPins[bit], (command >> (bit)) & 0x01);  // Write each bit of the command
  }
  // Clear the write register
  CLR_WR;
  // Set the write register
  SET_WR;
  // Deselect the LCD controller
  SET_CS;
}

void Display::write_data(uint8_t data) {
  // Select the LCD's data register
  CLR_DC;
  // Select the LCD controller
  CLR_CS;

  // Send the data via parallel to GPIO pins 0 to 7 on ESP32
  for (int bit = 0; bit <= 7; bit++) {
    digitalWrite(gpioPins[bit], (data >> (bit)) & 0x01);  // Write each bit of the data
  }

  // Clear the write register
  CLR_WR;
  // Set the write register
  SET_WR;
  // Deselect the LCD controller
  SET_CS;
}

void Display::cleardisplay() {
  int i = HRES * VRES / 8;

  write_data(0x00);
  write_data(0x08);
  write_command(0x24);

  write_command(0xB0);  // auto write

  while (i--) {
    write_data(0x00);
  }

  write_command(0xB2);  // auto write reset
}

void Display::cleartext() {
  write_data(0x00);
  write_data(0x00);
  write_command(0x24);

  write_command(0xB0);  // auto write

  for (int row = 0; row < VRES; row++) {
    for (int col = 0; col < HRES / 8; col++) {
      write_data(0x00);
    }
  }
  write_command(0xB2);  // auto write reset
}

void Display::display_bitmap(const uint8_t *display) {

  write_data(0x00);
  write_data(0x08);
  write_command(0x24);

  write_command(0x98);  // display mode - graphic on, text off
  write_command(0xB0);  // auto write
  int x = 0;

  for (int row = 0; row < VRES; row++) {
    for (int col = 0; col < HRES / 8; col++) {
      write_data(pgm_read_byte(&display[x]));
      x++;
      delay(0);
    }
  }
  write_command(0xB2);  // auto write reset
}

void Display::initialize_display() {
  SET_RESET;
  delay(10);
  CLR_RESET;
  delay(100);
  SET_RESET;
  delay(500);

  write_data(0x00);
  write_data(0x00);
  write_command(0x40);  // set text home address

  write_data(0x14);  // 240/8
  write_data(0x00);
  write_command(0x41);  // set text area

  write_data(0x00);
  write_data(0x08);     // address 0x0800
  write_command(0x42);  // set graphic home address

  write_data(HRES / 8);
  write_data(0x00);
  write_command(0x43);  // set graphic area

  write_command(0xA0);  // 1 line cursor
  write_command(0x81);  // mode set - exor mode
  write_command(0x9C);  // display mode - graphic on, text on

  write_data(0x00);
  write_data(0x00);
  write_command(0x21);  // set cursor position
}

void Display::GLCD_SetAddressPointer(unsigned int address) {
  write_data(address & 0xFF);
  write_data(address >> 8);
  write_command(T6963_SET_ADDRESS_POINTER);
}

void Display::GLCD_WriteDisplayData(unsigned char x) {
  write_data(x);
  write_command(T6963_DATA_WRITE_AND_INCREMENT);
}

void Display::GLCD_ClearCG(void) {
  unsigned int i;
  GLCD_SetAddressPointer(GLCD_EXTERNAL_CG_HOME);

  for (i = 0; i < 256 * 8; i++) {
    GLCD_WriteDisplayData(0);
  }
}

void Display::GLCD_ClearGraphic(void) {
  int i;
  GLCD_SetAddressPointer(GLCD_GRAPHIC_HOME);
  for (i = 0; i < GLCD_GRAPHIC_SIZE; i++) {
    GLCD_WriteDisplayData(0x00);
  }
}

void Display::GLCD_WriteString(String string)  // da sua tu
{
  const char *text = string.c_str();
  while (*text) {
    GLCD_WriteChar(*text++);
  }
}

void Display::GLCD_WriteChar(char charCode) {
  GLCD_WriteDisplayData(charCode - 32);
}

void Display::GLCD_TextGoTo(unsigned char x, unsigned char y) {
  unsigned int address;

  address = GLCD_TEXT_HOME + x + (GLCD_TEXT_AREA * y);

  GLCD_SetAddressPointer(address);
}
void Display::screenOn() {
  if (isPageChangeToSettingPage == 1) {
    cleartext();
    cleardisplay();
    isPageChangeToSettingPage = 0;
  }
  switch (currentPage) {
    case 0:
      settingPage(pool[lastPage]);
      break;
    default:
      homePage(pool[currentPage]);
      break;
  }
}

void Display::homePage(Pool A) {
  GLCD_TextGoTo(0, 0);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" HE THONG QUAN TRAC");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_TextGoTo(7, 2);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" BE ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.poolID));
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_TextGoTo(0, 5);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" MUC NUOC: ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.mucnuoc));
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" cm       ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_TextGoTo(0, 8);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" VAN VAO: ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  if (pool[currentPage].inStatus == 2 || pool[currentPage].inStatus == 3) {
    GLCD_WriteString("---    ");
    vTaskDelay(15 / portTICK_PERIOD_MS);
  } else {
    if (A.inStatus == 0) {
      GLCD_WriteString("OFF       ");
      vTaskDelay(15 / portTICK_PERIOD_MS);
    }
    if (A.inStatus == 1) {
      GLCD_WriteString("ON       ");
      vTaskDelay(15 / portTICK_PERIOD_MS);
    }
  }
  GLCD_TextGoTo(0, 11);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" VAN RA: ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  if (pool[currentPage].outStatus == 2 || pool[currentPage].outStatus == 3) {
    pool[currentPage].outStatus == 2 ? 1 : 0;
    GLCD_WriteString("---    ");
    vTaskDelay(15 / portTICK_PERIOD_MS);
  } else {
    if (A.outStatus == 0) {
      GLCD_WriteString("OFF       ");
      vTaskDelay(15 / portTICK_PERIOD_MS);
    }
    if (A.outStatus == 1) {
      GLCD_WriteString("ON       ");
      vTaskDelay(15 / portTICK_PERIOD_MS);
    }
  }
  GLCD_TextGoTo(0, 14);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" TU DONG: ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  if (pool[currentPage].autoStatus == 2 || pool[currentPage].autoStatus == 3) {
    GLCD_WriteString("---    ");
    vTaskDelay(15 / portTICK_PERIOD_MS);
  } else {
    if (A.autoStatus == 1) {
      GLCD_WriteString("ON       ");
      vTaskDelay(15 / portTICK_PERIOD_MS);
    }
    if (A.autoStatus == 0) {
      GLCD_WriteString("OFF       ");
      vTaskDelay(15 / portTICK_PERIOD_MS);
    }
  }
}
void Display::settingPage(Pool A) {

  GLCD_TextGoTo(0, 0);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString("  CAI DAT THONG SO  ");
  vTaskDelay(15 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(7, 2);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" BE ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.poolID));
  vTaskDelay(15 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(0, 5);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" CAO NHAT: ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.maxValue));
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" cm ");
  vTaskDelay(15 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(0, 8);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" GIUA: ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.midValue));
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" cm ");
  vTaskDelay(15 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(0, 11);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" THAP NHAT: ");
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.minValue));
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString(" cm ");
  vTaskDelay(15 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(0, 14);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  GLCD_WriteString("               ");
  vTaskDelay(15 / portTICK_PERIOD_MS);

  // hàm tạo con trỏ để cuối cùng của code này
  GLCD_TextGoTo(0, pointerPossitionAtSettingPage);
  vTaskDelay(15 / portTICK_PERIOD_MS);
  if (isChangingValueAtSettingPage == 1)
    GLCD_WriteChar('.');
  else
    GLCD_WriteChar('>');
  vTaskDelay(15 / portTICK_PERIOD_MS);
}

Button::Button() {}
void Button::begin() {
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(Left, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(select, INPUT_PULLUP);
  pinMode(supplyval, INPUT_PULLUP);
  pinMode(drainval, INPUT_PULLUP);
  pinMode(settingButton, INPUT_PULLUP);
  pinMode(autoButton, INPUT_PULLUP);
}

void Button::checkState() {
  //*************Page setting status***********
  if (digitalRead(settingButton) == 0) {
    if (currentPage == 0) {
      currentPage = lastPage;
      lastPage = 0;
    } else {
      lastPage = currentPage;
      pointerPossitionAtSettingPage = 5;
      isChangingValueAtSettingPage = 0;
      currentPage = 0;
    }
    isPageChangeToSettingPage = 1;
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  //*******************************
  if (currentPage != 0) {
    if (digitalRead(Right) == 0 && currentPage <= numberOfPool) {
      currentPage++;
      if (currentPage == numberOfPool + 1)
        currentPage = 1;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (digitalRead(Left) == 0 && currentPage <= numberOfPool) {
      currentPage--;
      if (currentPage == 0)
        currentPage = numberOfPool;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (digitalRead(autoButton) == 0) {
      if (pool[currentPage].autoStatus < 2)
        pool[currentPage].autoStatus = pool[currentPage].autoStatus == 0 ? 3 : 2;
      pool[currentPage].outStatus = 0;
      pool[currentPage].inStatus = 0;
      isValStatusButtonPressed = 1;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (pool[currentPage].autoStatus == 0) {
      if (digitalRead(supplyval) == 0) {
        if (pool[currentPage].inStatus < 2)
          pool[currentPage].inStatus = pool[currentPage].inStatus == 0 ? 3 : 2;
        isValStatusButtonPressed = 1;
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      if (digitalRead(drainval) == 0) {
        if (pool[currentPage].outStatus < 2)
          pool[currentPage].outStatus = pool[currentPage].outStatus == 0 ? 3 : 2;
        ;
        isValStatusButtonPressed = 1;
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    }
  }
  //*************Page setting status********************
  else if (currentPage == 0) {
    // number 15 and 5 is the maximum and smallest row of text display in lcd at setting page
    if (digitalRead(select) == 0) {
      isChangingValueAtSettingPage = !isChangingValueAtSettingPage;
      isSettingValChange++;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (isChangingValueAtSettingPage == 0) {
      if (digitalRead(up) == 0 && pointerPossitionAtSettingPage >= 5) {
        pointerPossitionAtSettingPage -= 3;
        if (pointerPossitionAtSettingPage < 5)
          pointerPossitionAtSettingPage = 11;
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      if (digitalRead(down) == 0 && pointerPossitionAtSettingPage <= 11) {
        pointerPossitionAtSettingPage += 3;
        if (pointerPossitionAtSettingPage > 11)
          pointerPossitionAtSettingPage = 5;
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    } else if (isChangingValueAtSettingPage == 1) {
      if (digitalRead(up) == 0) {
        if (pointerPossitionAtSettingPage == 5) {
          pool[lastPage].maxValue += 0.5;
        }
        if (pointerPossitionAtSettingPage == 8) {
          pool[lastPage].midValue += 0.5;
        }
        if (pointerPossitionAtSettingPage == 11) {
          pool[lastPage].minValue += 0.5;
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
      }
      if (digitalRead(down) == 0) {
        if (pointerPossitionAtSettingPage == 5 && pool[lastPage].maxValue > 0) {
          pool[lastPage].maxValue -= 0.5;
        }
        if (pointerPossitionAtSettingPage == 8 && pool[lastPage].midValue > 0) {
          pool[lastPage].midValue -= 0.5;
        }
        if (pointerPossitionAtSettingPage == 11 && pool[lastPage].minValue > 0) {
          pool[lastPage].minValue -= 0.5;
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
      }
    }
    //************************************************
  }
}
