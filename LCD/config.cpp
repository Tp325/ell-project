#include "config.h"
int gpioPins[8] = { 19, 21, 3, 26, 1, 27, 22, 23 };

//*********************** quản lý hồ****************
Pool pool[10];
void creatNewPool(int numberPool) {
  numberOfPool = numberPool;
  for (int i = 1; i <= numberPool; i++) {
    pool[i].poolID = i;
    pool[i].mucnuoc = 0.0;
    pool[i].maxValue = 0.0;
    pool[i].midValue = 0.0;
    pool[i].minValue = 0.0;
    pool[i].inStatus = 0;
    pool[i].outStatus = 0;
    pool[i].autoStatus = 0;
    pool[i].adrressOfSavedDataInEEPROM = 12 * i - 11;
  }
}
int numberOfPool = 0;

// *********** display page************
int currentPage = 1;  // page 0 : setting   page 1....n : pool display
int lastPage = 0;




// ***************button state************
bool isValStatusButtonPressed = 0;
uint8_t isSettingValChange = 0;
bool isPageChangeToSettingPage = 0;
int pointerPossitionAtSettingPage = 5;
bool isChangingValueAtSettingPage = 0;
bool haveDifferentValue = 0;

//********** communication**********
uint8_t IDOfPool;
String msgToSink;
String msgFromSink;
String bufferMsgFromSink;
String command;
String msgToEEPROM;


//**************CircularQueue************
CircularQueue *buffDataFromSink = createQueue(100);
CircularQueue *buffDataToSink = createQueue(100);
CircularQueue *buffDataToEEPROM = createQueue(100);
