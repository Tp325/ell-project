#include "Communication.h"
SPIClass spiLoRa(HSPI);
SX1278 radio = new Module(Nss, Dio0, Rst, Dio1, spiLoRa);
DynamicJsonDocument doc(1024);
WiFiManager wm;
WiFiClient espClient;
PubSubClient client(espClient);
Communication* Communication::instance = nullptr;
int state;
int trasmitState;
volatile bool receiveFlag = false;
void setReceiveFlag() {
  // Serial.println("flag");
  receiveFlag = true;
  if (isSended == 1) {
    receiveFlag = false;
    isSended = 0;
  }
}
Communication::Communication() {
}
void Communication::begin() {
  spiLoRa.begin(14, 12, 13, Nss);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  // Cài đặt driver UART2 với buffer RX 1024 byte và TX 1024 byte
  uart_driver_install(UART_NUM_2, 1024, 1024, 0, NULL, 0);
  Serial.println("[SX1278] Initializing ... ");
  state = radio.begin(carrierFrequency, bandwidth, spreadingFactor, codingRate, syncWord, outputPower, preambleLength, amplifierGain);
  // state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("success!");
  } else {
    Serial.print("failed, code ");
    Serial.println(state);
    while (true) { vTaskDelay(100 / portTICK_PERIOD_MS); }
  }
  radio.setPacketReceivedAction(setReceiveFlag);
  Serial.print(F("[SX1278] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }
  WiFi.mode(WIFI_STA);
  wm.setConfigPortalBlocking(false);
  if (wm.autoConnect("EllPoolWiFi")) {
    Serial.println("connected...yeey :)");
  } else {
    Serial.println("Configportal running");
  }
}

void Communication::receiveFromDisplay() {
  if (Serial2.available()) {
    msgFromDisplay = Serial2.readStringUntil('\n');
    Serial.print("receive from display: ");
    Serial.println(msgFromDisplay);
    if (!isFull(buffDataFromDisplay)) {
      enqueueData(buffDataFromDisplay, msgFromDisplay.c_str());
    }
  }
}
void Communication::sendToNode() {
  while (!isEmpty(buffDataFromDisplay)) {
    msgToNode = dequeue(buffDataFromDisplay);
    isSended = 1;
    trasmitState = radio.transmit(msgToNode);
    if (trasmitState == RADIOLIB_ERR_NONE) {
      Serial.print("send to node: ");
      Serial.println(msgToNode);
      Serial.println("transmission finished!");
    } else {
      Serial.print("failed, code ");
      Serial.println(state);
    }
    vTaskDelay(5 / portTICK_PERIOD_MS);
    state = radio.startReceive();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void Communication::receiveFromNode() {
  if (receiveFlag) {
    receiveFlag = false;
    state = radio.readData(msgFromNode);
    if (state == RADIOLIB_ERR_NONE) {
      if (!isFull(buffDataFromNode)) {
        Serial.print("receive From Node: ");
        Serial.println(msgFromNode);
        enqueueData(buffDataFromNode, msgFromNode.c_str());
      }
      if (!isFull(buffDataToServer)) {
        enqueueData(buffDataToServer, msgFromNode.c_str());
      }
      msgFromNode = "";
    }
  }
}
void Communication::sendToDisplay() {
  while (!isEmpty(buffDataFromNode)) {
    msgToDisplay = "";
    Serial.print("send to display: ");
    msgToDisplay = dequeue(buffDataFromNode);
    Serial.println(msgToDisplay);
    Serial2.println(msgToDisplay);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
void Communication::sendToServer() {
  while (!isEmpty(buffDataToServer)) {
    msgToServer = "";
    Serial.print("send to server: ");
    msgToServer = dequeue(buffDataToServer);
    Serial.println(msgToServer);
    client.publish(topicSend.c_str(), msgToServer.c_str());
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}
void Communication::receiveFromServer() {
  if (msgFromServer != "") {
    if (!isFull(buffDataFromDisplay)) {
      enqueueData(buffDataFromDisplay, msgFromServer.c_str());
    }
    Serial.print("receive From Server: ");
    Serial.println(msgFromServer);
    msgFromServer = "";
  }
}

void Communication::processWiFi() {
  wm.process();
  if (WiFi.status() == WL_CONNECTED) {
    if (isWifiConnect == 0) {
      client.setServer(mqtt_server, mqtt_port);
      client.setCallback(callbackWrapper);
      connectMqtt();
      isWifiConnect = 1;
    }
  } else {
    isWifiConnect = 0;
  }
}
void Communication::connectMqtt() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("EllPool", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(topicReceive.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Communication::processMQTT() {
  if (!client.connected()) {
    connectMqtt();
  }
  client.loop();
}


void Communication::callbackmqtt(char* topic, byte* message, unsigned int length) {
  msgFromServer = "";
  for (int i = 0; i < length; i++) {
    msgFromServer += (char)message[i];
  }
}