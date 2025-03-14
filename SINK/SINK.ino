// SINK.ino
#include "Communication.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

Communication communication;

void setup() {
  Serial.begin(9600);
  communication.begin();
  xTaskCreate(vtaskSend, "taskSend", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskRecive, "taskRecive", 20000, NULL, 5, NULL);
  xTaskCreate(vtaskMqtt, "taskMqtt", 30000, NULL, 5, NULL);
  vTaskDelete(NULL);
}

void loop() {
}

void vtaskSend(void *pvParameters) {
  while (1) {
    communication.sendToNode();
    communication.sendToDisplay();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void vtaskRecive(void *pvParameters) {
  while (1) {
    communication.receiveFromDisplay();
    communication.receiveFromNode();
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
void vtaskMqtt(void *pvParameters) {
  communication.connnectToWifi(ssid, password);
  communication.connnectToMQTT(mqttServer, mqttTopicSubscribe);
  while (1) {
    communication.processMqtt();
    communication.mqttReconnect(mqttTopicSubscribe);
    communication.analizeDataFromMQTT();
    if (!isEmpty(buffDataToMqtt)) {
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
