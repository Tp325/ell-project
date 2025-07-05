#include "Display.h"
Display::Display(int clock, int data, int CS, int reset, int enableLed) {
  u8g2 = new U8G2_ST7920_128X64_F_SW_SPI(U8G2_R0, clock, data, CS, reset);
  this->enableLed = enableLed;
}
void Display::begin() {
  u8g2->begin();
  pinMode(enableLed, OUTPUT);
  digitalWrite(enableLed, HIGH);
  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_bubble_tr);
  u8g2->drawStr((128 - u8g2->getStrWidth("IOT LAB")) / 2, 40, "IOT LAB");
  u8g2->sendBuffer();
  delay(3000);
  u8g2->setFont(u8g2_font_6x12_tr);
}
void Display::sleepDisplay() {
  digitalWrite(enableLed, LOW);
}
void Display::onDisplay() {
  digitalWrite(enableLed, HIGH);
}

void Display::screenOn() {
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
  u8g2->clearBuffer();
  u8g2->drawStr((128 - u8g2->getStrWidth("HE THONG QUAN TRAC")) / 2, 10, "HE THONG QUAN TRAC");
  u8g2->drawStr((128 - u8g2->getStrWidth(String(String("BE ") + String(A.poolID)).c_str())) / 2, 20, String(String("BE ") + String(A.poolID)).c_str());
  u8g2->drawStr(10, 30, "MUC NUOC:");
  if (int(A.mucnuoc) == 404)
    u8g2->drawStr(65, 30, "404 Error");
  else {
    u8g2->drawStr(65, 30, String(A.mucnuoc).c_str());
    u8g2->drawStr(110, 30, "cm");
  }
  u8g2->drawStr(10, 40, "VAN VAO:");
  if (A.inStatus == 2 || A.inStatus == 3) {
    u8g2->drawStr(65, 40, "---");
  } else {
    if (A.inStatus == 0) {
      u8g2->drawStr(65, 40, "OFF");
    }
    if (A.inStatus == 1) {
      u8g2->drawStr(65, 40, "ON ");
    }
  }
  u8g2->drawStr(10, 50, "VAN RA:");
  if (A.outStatus == 2 || A.outStatus == 3) {
    u8g2->drawStr(65, 50, "---");
  } else {
    if (A.outStatus == 0) {
      u8g2->drawStr(65, 50, "OFF");
    }
    if (A.outStatus == 1) {
      u8g2->drawStr(65, 50, "ON ");
    }
  }
  u8g2->drawStr(10, 60, "TU DONG:");
  if (A.autoStatus == 2 || A.autoStatus == 3) {
    u8g2->drawStr(65, 60, "---    ");
  } else {
    if (A.autoStatus == 4) {
      u8g2->drawStr(65, 60, "ERROR   ");
      A.autoStatus = 0;
    } else {
      if (A.autoStatus == 1) {
        u8g2->drawStr(65, 60, "PROCESSING");
      }
      if (A.autoStatus == 0) {
        u8g2->drawStr(65, 60, "OFF    ");
      }
    }
  }
  u8g2->sendBuffer();
}

void Display::settingPage(Pool A) {
  u8g2->clearBuffer();
  u8g2->drawStr((128 - u8g2->getStrWidth("CAI DAT THONG SO")) / 2, 10, "CAI DAT THONG SO");
  u8g2->drawStr((128 - u8g2->getStrWidth(String(String("BE ") + String(A.poolID)).c_str())) / 2, 20, String(String("BE ") + String(A.poolID)).c_str());
  u8g2->drawStr(10, 30, "CAO :");
  u8g2->drawStr(40, 30, String(A.maxValue).c_str());
  u8g2->drawStr(90, 30, "cm");
  u8g2->drawStr(10, 40, "GIUA :");
  u8g2->drawStr(40, 40, String(A.midValue).c_str());
  u8g2->drawStr(90, 40, "cm");
  u8g2->drawStr(10, 50, "THAP :");
  u8g2->drawStr(40, 50, String(A.minValue).c_str());
  u8g2->drawStr(90, 50, "cm");
  // // hàm tạo con trỏ để cuối cùng của code này
  if (isChangingValueAtSettingPage == 1) {
    u8g2->drawStr(5, pointerPossitionAtSettingPage, ".");
  } else {
    u8g2->drawStr(5, pointerPossitionAtSettingPage, ">");
  }
  u8g2->sendBuffer();
}