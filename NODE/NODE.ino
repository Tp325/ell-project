#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Communication.h"
#include "Execution.h"
#include "Sensor.h"
Execution execution;
Communication communication;
RS485Sensor sensor;

void setup() {
  createNewPool(1, VALVE_SUPPLY_1, VALVE_DRAIN_1, 0x11, 60);
  createNewPool(2, VALVE_SUPPLY_2, VALVE_DRAIN_2, 0x22, 60);
  createNewPool(3, VALVE_SUPPLY_3, VALVE_DRAIN_3, 0x33, 60);
  numberOfPool = 3;
  Serial.begin(9600);
  communication.begin();
  execution.begin();
  sensor.begin();
  xTaskCreate(vTaskReceive, "TaskReceive", 20000, NULL, 5, NULL);
  xTaskCreate(vTaskSend, "TaskSend", 20000, NULL, 5, NULL);
  xTaskCreate(vTaskAnalize, "TaskAnalize", 20000, NULL, 5, NULL);
  xTaskCreate(vTaskExecution, "TaskExecution", 20000, NULL, 5, NULL);
  xTaskCreate(vTaskReadSensor, "TaskReadSensor", 20000, NULL, 5, NULL);
  vTaskDelete(NULL);
}

void loop() {
}
void vTaskAnalize(void *pvParameters) {
  while (1) {
    communication.analizeData();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vTaskReceive(void *pvParameters) {
  while (1) {
    communication.receiveFromSink();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vTaskSend(void *pvParameters) {
  while (1) {
    communication.sendToSink();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
void vTaskExecution(void *pvParameters) {
  while (1) {
    for (int i = 1; i <= numberOfPool; i++) {
      if (pool[i].autoStatus == 1) {
        execution.autoRun(i);
        if (pool[i].isDoneAutoMode == 1) {
          pool[i].isDoneAutoMode = 0;
          communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"a\":" + String(int(pool[i].autoStatus)) + "}"));
          communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"mucn\":" + String(pool[i].mucnuoc) + "}"));
        }
        if (pool[i].sentStatus == 1) {
          communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"i\":" + String(int(pool[i].inStatus)) + "}"));
          communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"o\":" + String(int(pool[i].outStatus)) + "}"));
          pool[i].sentStatus = 0;
        }
      } else {
        if (pool[i].inStatus == 1) {
          execution.supplyIn(i);
        } else {
          execution.xsupplyIn(i);
        }
        if (pool[i].outStatus == 1) {
          execution.drainOut(i);
        } else {
          execution.xdrainOut(i);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
void vTaskReadSensor(void *pvParameters) {
  for (int i = 1; i <= numberOfPool; i++) {
    pool[i].mucnuoc = pool[i].SensorpieLenght - sensor.getSensorValue(pool[i].IDOfSensor) / 10.0;
    communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"mucn\":" + String(pool[i].mucnuoc) + "}"));
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
  while (1) {
    for (int i = 1; i <= numberOfPool; i++) {
      if (pool[i].autoStatus == 1) {
        if (millis() - pool[i].timeReposeDataSensorToSink >= 3000) {
          pool[i].mucnuoc = pool[i].SensorpieLenght - sensor.getSensorValue(pool[i].IDOfSensor) / 10.0;
          communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"mucn\":" + String(pool[i].mucnuoc) + "}"));
          pool[i].timeReposeDataSensorToSink = millis();
        }
      } else if (millis() - pool[i].timeReposeDataSensorToSink >= 60 * 1000) {
        pool[i].mucnuoc = pool[i].SensorpieLenght - (sensor.getSensorValue(pool[i].IDOfSensor)) / 10.0;
        communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"mucn\":" + String(pool[i].mucnuoc) + "}"));
        pool[i].timeReposeDataSensorToSink = millis();
      }
      vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}