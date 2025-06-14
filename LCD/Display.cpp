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

void Display::homePage(Pool &A) {
  GLCD_TextGoTo(0, 0);
  GLCD_WriteString(" HE THONG QUAN TRAC");
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_TextGoTo(7, 2);
  GLCD_WriteString(" BE ");
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.poolID));
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_TextGoTo(0, 5);
  GLCD_WriteString(" MUC NUOC: ");
  if (int(A.mucnuoc) == 404)
    GLCD_WriteString("404 Error");
  else {
    GLCD_WriteString(String(A.mucnuoc));
    vTaskDelay(5 / portTICK_PERIOD_MS);
    GLCD_WriteString(" cm       ");
  }
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_TextGoTo(0, 8);
  GLCD_WriteString(" VAN VAO: ");
  if (A.inStatus == 2 || A.inStatus == 3) {
    vTaskDelay(5 / portTICK_PERIOD_MS);
    GLCD_WriteString("---    ");
    vTaskDelay(5 / portTICK_PERIOD_MS);
  } else {
    if (A.inStatus == 0) {
      vTaskDelay(5 / portTICK_PERIOD_MS);
      GLCD_WriteString("OFF       ");
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    if (A.inStatus == 1) {
      vTaskDelay(5 / portTICK_PERIOD_MS);
      GLCD_WriteString("ON       ");
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
  GLCD_TextGoTo(0, 11);
  GLCD_WriteString(" VAN RA: ");
  if (A.outStatus == 2 || A.outStatus == 3) {
    vTaskDelay(5 / portTICK_PERIOD_MS);
    GLCD_WriteString("---    ");
    vTaskDelay(5 / portTICK_PERIOD_MS);
  } else {
    if (A.outStatus == 0) {
      vTaskDelay(5 / portTICK_PERIOD_MS);
      GLCD_WriteString("OFF       ");
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    if (A.outStatus == 1) {
      vTaskDelay(5 / portTICK_PERIOD_MS);
      GLCD_WriteString("ON       ");
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }
  }
  GLCD_TextGoTo(0, 14);
  GLCD_WriteString(" TU DONG: ");
  if (A.autoStatus == 2 || A.autoStatus == 3) {
    vTaskDelay(5 / portTICK_PERIOD_MS);
    GLCD_WriteString("---    ");
    vTaskDelay(5 / portTICK_PERIOD_MS);
  } else {
    if (A.autoStatus == 4) {
      vTaskDelay(5 / portTICK_PERIOD_MS);
      GLCD_WriteString("ERROR  ");
      vTaskDelay(300 / portTICK_PERIOD_MS);
      A.autoStatus = 0;
    } else {
      if (A.autoStatus == 1) {
        vTaskDelay(5 / portTICK_PERIOD_MS);
        GLCD_WriteString("PROCESSING       ");
        vTaskDelay(5 / portTICK_PERIOD_MS);
      }
      if (A.autoStatus == 0) {
        vTaskDelay(5 / portTICK_PERIOD_MS);
        GLCD_WriteString("OFF       ");
        vTaskDelay(5 / portTICK_PERIOD_MS);
      }
    }
  }
}
void Display::settingPage(Pool A) {

  GLCD_TextGoTo(0, 0);
  GLCD_WriteString("  CAI DAT THONG SO  ");
  vTaskDelay(5 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(7, 2);
  GLCD_WriteString(" BE ");
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.poolID));
  vTaskDelay(5 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(0, 5);
  GLCD_WriteString(" CAO NHAT: ");
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.maxValue));
  GLCD_WriteString(" cm ");
  vTaskDelay(5 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(0, 8);
  GLCD_WriteString(" GIUA: ");
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.midValue));
  GLCD_WriteString(" cm ");
  vTaskDelay(5 / portTICK_PERIOD_MS);

  GLCD_TextGoTo(0, 11);
  GLCD_WriteString(" THAP NHAT: ");
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_WriteString(String(A.minValue));
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_WriteString(" cm ");
  vTaskDelay(5 / portTICK_PERIOD_MS);
  GLCD_TextGoTo(0, 14);
  GLCD_WriteString("               ");
  vTaskDelay(5 / portTICK_PERIOD_MS);

  // hàm tạo con trỏ để cuối cùng của code này
  GLCD_TextGoTo(0, pointerPossitionAtSettingPage);
  vTaskDelay(5 / portTICK_PERIOD_MS);
  if (isChangingValueAtSettingPage == 1) {
    vTaskDelay(5 / portTICK_PERIOD_MS);
    GLCD_WriteChar('.');
  } else {
    vTaskDelay(5 / portTICK_PERIOD_MS);
    GLCD_WriteChar('>');
  }
  vTaskDelay(5 / portTICK_PERIOD_MS);
}