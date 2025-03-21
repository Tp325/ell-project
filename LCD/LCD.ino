#include "Display.h"
#include "Communication.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
Display screen;
Communication communication;
Button button;
void setup() {
  creatNewPool(3);
  Serial.begin(9600);
  communication.begin();
  screen.begin();
  xTaskCreatePinnedToCore(vtaskButton, "taskButton", 20000, NULL, 5, NULL, 0);
  xTaskCreate(vtaskDisplay, "taskDisplay", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskSensorDetect, "taskSensorDetect", 4096, NULL, 5, NULL);
  xTaskCreate(vtaskSendToSink, "taskSendToSink", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskReceiveFromSink, "taskReceiveFromSink", 10000, NULL, 5, NULL);

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
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
void vtaskSendToSink(void *pvParameters) {
  while (1) {
    if (isValStatusButtonPressed == 1 || isSettingValChange == 2) {
      communication.sendToSink();
      vTaskDelay(100 / portTICK_PERIOD_MS);
      isValStatusButtonPressed = 0;
      isSettingValChange = 0;
    }
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