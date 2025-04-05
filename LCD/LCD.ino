#include "Display.h"
#include "Communication.h"
#include "Storage.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
Display screen;
Communication communication;
Button button;
Storage storage;
void setup() {
  creatNewPool(3);
  communication.begin();
  screen.begin();
  storage.begin();
  delay(1000);
  xTaskCreatePinnedToCore(vtaskButton, "taskButton", 20000, NULL, 5, NULL, 0);
  xTaskCreate(vtaskDisplay, "taskDisplay", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskSensorDetect, "taskSensorDetect", 4096, NULL, 5, NULL);
  xTaskCreate(vtaskSendToSink, "taskSendToSink", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskReceiveFromSink, "taskReceiveFromSink", 20000, NULL, 5, NULL);
  xTaskCreate(vTaskAnalize, "TaskAnalize", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskAnalizeDataToSink, "taskAnalizeDataToSink", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskSaveToEEPROM, "taskSaveToEEPROM", 20000, NULL, 5, NULL);
  vTaskDelete(NULL);
}
void loop() {
}
void vtaskButton(void *pvParameters) {
  button.begin();
  while (1) {
    button.checkState();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vtaskDisplay(void *pvParameters) {
  while (1) {
    screen.screenOn();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vtaskSaveToEEPROM(void *pvParameters) {
  while (1) {
    storage.saveToEEPROM();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}
void vtaskAnalizeDataToSink(void *pvParameters) {
  while (1) {
    if (isValStatusButtonPressed == 1 || isSettingValChange == 2) {
      communication.analizeDataToSink();
      if (isSettingValChange == 2) {
        if (!isFull(buffDataToEEPROM)) {
          msgToEEPROM = String(lastPage);
          enqueueData(buffDataToEEPROM, msgToEEPROM.c_str());
        }
        isSettingValChange = 0;
      }
      isValStatusButtonPressed = 0;
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vtaskSendToSink(void *pvParameters) {
  while (1) {
    communication.sendToSink();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vtaskReceiveFromSink(void *pvParameters) {
  String msgBuffer;
  while (1) {
    communication.receiveFromSink();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vTaskAnalize(void *pvParameters) {
  while (1) {
    communication.analizeData();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
// đưa màng hình vào chế độ ngủ
void vtaskSensorDetect(void *parameter) {
  uint8_t time = 0;
  pinMode(SS_DETECT, INPUT);
  while (1) {
    if (digitalRead(SS_DETECT) == 1) {
      time = 0;
      digitalWrite(LED_LCD, HIGH);
    } else {
      if (time >= 15)
        digitalWrite(LED_LCD, LOW);
    }
    time++;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
