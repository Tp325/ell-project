#include "Communication.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
Communication communication;
void setup() {
  Serial.begin(9600);
  communication.begin();
  xTaskCreate(vtaskSendToNode, "taskSendToNode", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskReciveFromDisplay, "taskReciveFromDisplay", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskReceiveFromNode, "taskReceiveFromNode", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskSendToDisplay, "taskSendToDisplay", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskSendToServer, "taskSendToServer", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskReceiveFromServer, "taskReceiveFromServer", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskProcessWiFi, "taskProcessWiFi", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskProcessMQTT, "taskProcessMQTT", 10000, NULL, 5, NULL);
  xTaskCreate(vtaskSynchronize, "taskSynchronize", 5000, NULL, 5, NULL);
  vTaskDelete(NULL);
}

void loop() {
}
void vtaskSynchronize(void *pvParameters) {
  while (1) {
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    if (!isFull(buffDataFromDisplay)) {
      enqueueData(buffDataFromDisplay, String("{\"is\":1,\"cm\":\"GD\"}").c_str());
    }
    vTaskDelete(NULL);
  }
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
void vtaskReceiveFromServer(void *pvParameters) {
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  if (!isFull(buffDataFromDisplay)) {
    enqueueData(buffDataFromDisplay, String("{\"is\":1,\"cm\":\"GD\"}").c_str());
  }
  while (1) {
    communication.receiveFromServer();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
void vtaskSendToServer(void *pvParameters) {
  while (1) {
    communication.sendToServer();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
void vtaskProcessWiFi(void *pvParameters) {
  while (1) {
    communication.processWiFi();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
void vtaskProcessMQTT(void *pvParameters) {
  while (1) {
    communication.processMQTT();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}