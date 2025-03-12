#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Communication.h"
#include "Execution.h"
Communication communication;
Execution execution;
void setup() {
  createNewPool(1, VALVE_SUPPLY_1, VALVE_DRAIN_1, 0x11);
  createNewPool(2, VALVE_SUPPLY_2, VALVE_DRAIN_2, 0x22);
  createNewPool(3, VALVE_SUPPLY_3, VALVE_DRAIN_3, 0x33);
  numberOfPool = 3;
  Serial.begin(9600);
  communication.begin();
  xTaskCreate(vTaskReceive, "TaskReceive", 10000, NULL, 5, NULL);
  xTaskCreate(vTaskAnalizeData, "TaskAnalizeData", 10000, NULL, 5, NULL);
  xTaskCreate(vTaskExecution, "TaskExecution", 10000, NULL, 5, NULL);
  // xTaskCreate(vTaskSend, "TaskSend", 20000, NULL, 5, NULL);
}
void loop() {
  vTaskDelete(NULL);
}
void vTaskReceive(void *pvParameters) {
  while (1) {
    communication.receiveFromSink();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
void vTaskSend(void *pvParameters) {
  while (1) {
    communication.sendToSink();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void vTaskAnalizeData(void *pvParameters) {
  while (1) {
    communication.analysis_send_DataToSink();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void vTaskExecution(void *pvParameters) {
  execution.begin();
  while (1) {
    if (pool[IDOfPool].inStatus == 1) {
      execution.supplyIn(IDOfPool);
    } else {
      execution.xsupplyIn(IDOfPool);
    }
    if (pool[IDOfPool].outStatus == 1) {
      execution.drainOut(IDOfPool);
    } else {
      execution.xdrainOut(IDOfPool);
    }
    if (pool[IDOfPool].autoStatus == 1) {

    } else {
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}