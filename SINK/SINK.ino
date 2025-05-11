#include "Communication.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
Communication communication;
void setup() {
  Serial.begin(9600);
  communication.begin();
  xTaskCreatePinnedToCore(vtaskSendToNode, "taskSendToNode", 8192, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskReceiveFromNode, "taskReceiveFromNode", 8192, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskReciveFromDisplay, "taskReciveFromDisplay", 2048, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskSendToDisplay, "taskSendToDisplay", 2048, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskSendToServer, "taskSendToServer", 2048, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskReceiveFromServer, "taskReceiveFromServer", 2048, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskProcessWiFi, "taskProcessWiFi", 8192, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskProcessMQTT, "taskProcessMQTT", 2048, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskSynchronize, "taskSynchronize", 2048, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskBlocking, "taskBlocking", 4096, NULL, 5, NULL, 0);
  vTaskDelete(NULL);
}

void loop() {
  vTaskDelay(10 / portTICK_PERIOD_MS);
}
void vtaskSynchronize(void *pvParameters) {
  vTaskDelay(5000 / portTICK_PERIOD_MS);
  if (!isFull(buffDataFromDisplay)) {
    enqueueData(buffDataFromDisplay, String("{\"is\":1,\"SID\":\"" + String(StationID) + "\",\"cm\":\"GD\"}").c_str());
  }
  vTaskDelete(NULL);
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
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
void vtaskProcessMQTT(void *pvParameters) {
  while (1) {
    communication.processMQTT();
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
void vtaskBlocking(void *pvParameters) {
  while (1) {
    // reconnect wifi && mqtt reconnnect
    communication.reconnectWifi();
    communication.reconnectMQTT();
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}