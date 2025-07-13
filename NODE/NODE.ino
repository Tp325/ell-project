#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Communication.h"
#include "Execution.h"
#include "Sensor.h"
Execution execution;
Communication communication;
RS485Sensor sensor;
//***** for testing time respose***********
// bool isStartCount = 0;
// int number = 0;
// unsigned long int countTime = 0;
//*********************************************
void setup() {
  // StationID = "TESTH6CT";
  numberOfPool = 3;
  createNewPool(1, VALVE_SUPPLY_1, VALVE_DRAIN_1, 0x11, 80);
  createNewPool(2, VALVE_SUPPLY_2, VALVE_DRAIN_2, 0x22, 80);
  createNewPool(3, VALVE_SUPPLY_3, VALVE_DRAIN_3, 0x33, 80);
  Serial.begin(9600);
  communication.begin();
  execution.begin();
  sensor.begin();
  xTaskCreatePinnedToCore(vTaskReceive, "TaskReceive", 6144, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vTaskSend, "TaskSend", 6144, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vTaskAnalize, "TaskAnalize", 6144, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vTaskExecution, "TaskExecution", 6144, NULL, 5, NULL, 0);
  xTaskCreatePinnedToCore(vTaskExecutionAutoRun, "TaskExecutionAutoRun", 6144, NULL, 5, NULL, 0);
  xTaskCreatePinnedToCore(vTaskReadSensor, "TaskReadSensor", 4096, NULL, 5, NULL, 1);
  xTaskCreatePinnedToCore(vTaskBlocking, "TaskBlocking", 2048, NULL, 5, NULL, 0);
  vTaskDelete(NULL);
}
void loop() {
}
void vTaskBlocking(void *pvParameters) {
  while (1) {
    if (isEmpty(buffDataToSink) && haveToReset == 1) {
      ESP.restart();
    }
    // for (int i = 1; i <= numberOfPool; i++) {
    //   Serial.printf("ID:%d\n autoStatus:%d\t stepOfAuto:%d\t inStatus:%d\t outStatus:%d\n", i, pool[i].autoStatus, pool[i].stepOfAuto, pool[i].inStatus, pool[i].outStatus);
    //   Serial.printf("ID:%d\n muc nuoc:%f\t max:%f\t mid:%f\t min:%f\n", i, pool[i].mucnuoc, pool[i].maxValue, pool[i].midValue, pool[i].minValue);
    //   Serial.println("________________________________");
    // }
    vTaskDelay(3000 / portTICK_PERIOD_MS);
  }
}
void vTaskAnalize(void *pvParameters) {
  while (1) {
    for (int i = 1; i <= numberOfPool; i++) {
      if (pool[i].isDoneAutoMode == 1) {
        communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"a\":" + String(int(pool[i].autoStatus)) + ",\"i\":" + String(int(pool[i].inStatus)) + ",\"o\":" + String(int(pool[i].outStatus)) + "}"));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"mucn\":" + String(pool[i].mucnuoc) + "}"));
        pool[i].isDoneAutoMode = 0;
      }
      if (pool[i].isSentValStatus == 1) {
        communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"i\":" + String(int(pool[i].inStatus)) + "}"));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"o\":" + String(int(pool[i].outStatus)) + "}"));
        pool[i].isSentValStatus = 2;
      }
    }
    communication.analizeData();
    vTaskDelay(100 / portTICK_PERIOD_MS);
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
    //***** for testing time respose***********
    // if (isStartCount) {
    //   Serial.printf("Count time no: %d \t result: %ld \n", number, millis() - countTime);
    //   number++;
    //   isStartCount = 0;
    // }
    //****************************************
    for (int i = 1; i <= numberOfPool; i++) {
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
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}
void vTaskExecutionAutoRun(void *pvParameters) {
  while (1) {
    for (int i = 1; i <= numberOfPool; i++) {
      if (pool[i].autoStatus == 1) {
        execution.autoRun(i);
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void vTaskReadSensor(void *pvParameters) {
  vTaskDelay(10000 / portTICK_PERIOD_MS);
  for (int i = 1; i <= numberOfPool; i++) {
    pool[i].mucnuoc = sensor.getDistance(pool[i].IDOfSensor, pool[i].SensorpieLenght);
    communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"mucn\":" + String(pool[i].mucnuoc) + "}"));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  while (1) {
    for (int i = 1; i <= numberOfPool; i++) {
      if (pool[i].autoStatus == 1) {
        if (millis() - pool[i].timeReposeDataSensorToSink >= 15000) {
          pool[i].mucnuoc = sensor.getDistance(pool[i].IDOfSensor, pool[i].SensorpieLenght);
          communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"mucn\":" + String(pool[i].mucnuoc) + "}"));
          pool[i].timeReposeDataSensorToSink = millis();
        }
      } else if (millis() - pool[i].timeReposeDataSensorToSink >= 60 * 1000) {
        pool[i].mucnuoc = sensor.getDistance(pool[i].IDOfSensor, pool[i].SensorpieLenght);
        communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"mucn\":" + String(pool[i].mucnuoc) + "}"));
        pool[i].timeReposeDataSensorToSink = millis();
      }
      if (int(pool[i].mucnuoc) == 404) {
        if (pool[i].isSentSensorStatus == 0) {
          pool[i].autoStatus = 0;
          pool[i].stepOfAuto = 0;
          pool[i].inStatus = 0;
          pool[i].outStatus = 0;
          pool[i].isDoneAutoMode = 0;
          communication.sendToSink(String("{\"is\":0,\"SID\":\"" + String(StationID) + "\",\"ID\":" + String(i) + ",\"a\":0,\"i\":0,\"o\":0}"));
        }
        pool[i].isSentSensorStatus = 1;
      } else {
        pool[i].isSentSensorStatus = 0;
      }
      //***** for testing time respose***********
      // countTime = millis();`
      // isStartCount = 1;
      //****************************************
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}