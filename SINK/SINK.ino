#include "Communication.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
Communication communication;
void setup() {
  Serial.begin(9600);
  communication.begin();
  delay(2000);
  // xTaskCreatePinnedToCore(vTaskPrintDebug, "TaskPrintDebug", 1024, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskSendToNode, "taskSendToNode", 6144, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskReceiveFromNode, "taskReceiveFromNode", 6144, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskReciveFromDisplay, "taskReciveFromDisplay", 4096, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskSendToDisplay, "taskSendToDisplay", 4096, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskSendToServer, "taskSendToServer", 4096, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskReceiveFromServer, "taskReceiveFromServer", 4096, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vtaskProcessWiFi, "taskProcessWiFi", 4096, NULL, 5, NULL, 0);
  xTaskCreatePinnedToCore(vtaskProcessMQTT, "taskProcessMQTT", 4096, NULL, 5, NULL, 0);
  xTaskCreatePinnedToCore(vtaskSynchronize, "taskSynchronize", 1024, NULL, 5, NULL, 0);
  xTaskCreatePinnedToCore(vtaskBlocking, "taskBlocking", 4096, NULL, 5, NULL, 0);
  vTaskDelete(NULL);
}

void loop() {
}
// void vTaskPrintDebug(void *pvParameters) {
//   size_t freeHeap = xPortGetFreeHeapSize();
//   while (1) {
//     printf("Free heap size: %u bytes\n", (unsigned int)freeHeap);
//     vTaskDelay(3000 / portTICK_PERIOD_MS);
//   }
// }
void vtaskSynchronize(void *pvParameters) {
  vTaskDelay(10000 / portTICK_PERIOD_MS);
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
    if (isEmpty(buffDataFromNode) && haveToReset == 1) {
      ESP.restart();
    }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}