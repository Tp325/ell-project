#include "Communication.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
Communication communication;
void setup() {
  Serial.begin(9600);
  communication.begin();
  xTaskCreate(vtaskSendToNode, "taskSendToNode", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskReciveFromDisplay, "taskReciveFromDisplay", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskReceiveFromNode, "taskReceiveFromNode", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskSendToDisplay, "taskSendToDisplay", 20000, NULL, 5, NULL);
  vTaskDelete(NULL);
}

void loop() {
}
void vtaskSendToNode(void *pvParameters) {
  while (1) {
    communication.sendToNode();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vtaskReceiveFromNode(void *pvParameters) {
  while (1) {
    communication.receiveFromNode();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vtaskReciveFromDisplay(void *pvParameters) {
  while (1) {
    communication.receiveFromDisplay();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vtaskSendToDisplay(void *pvParameters) {
  while (1) {
    communication.sendToDisplay();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}