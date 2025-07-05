#ifndef config_h
#define config_h
#include <Arduino.h>
#include "CircularQueue.h"

//********************* các chân nút ấn ****************
#define Left 33   //đổi thành L thường thì bị trùng macro với thư viện json
#define Right 13  //đổi thành R thường thì bị trùng macro với thư viện json
#define up 32
#define down 14
#define select 25
#define supplyval 36      // ON/OFF supply val
#define drainval 39       // ON/OFF drain val
#define settingButton 34  // SETTING
#define autoButton 35     // Tự động - thủ công
#define SS_DETECT 15

extern String StationID ;

//*********************** quản lý hồ****************
extern int numberOfPool;
struct Pool {
  int poolID;
  float mucnuoc;
  uint8_t inStatus;
  uint8_t outStatus;
  uint8_t autoStatus;
  float maxValue;
  float midValue;
  float minValue;
  int addressOfSavedDataInEEPROM;
};
extern Pool pool[10];
extern void creatNewPool(int numberPool);

// *********** display page************
extern int currentPage;  // page 0 : setting   page 1....n : pool display
extern int lastPage;


// ***************button state************
extern bool isValStatusButtonPressed;
extern uint8_t isSettingValChange;
extern int pointerPossitionAtSettingPage;
extern bool isChangingValueAtSettingPage;
extern bool haveDifferentValue;
//********** communication**********

extern uint8_t IDOfPool;
extern String msgToSink;
extern String msgFromSink;
extern String bufferMsgFromSink;
extern String msgToEEPROM;

//***********command**************
//GD mean getdata
extern String command;
extern bool haveToReset;

//************* CircularQueue*****************
extern CircularQueue *buffDataFromSink;
extern CircularQueue *buffDataToSink;
extern CircularQueue *buffDataToEEPROM;


#endif