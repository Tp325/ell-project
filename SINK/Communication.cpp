#include "Communication.h"
DynamicJsonDocument doc(1024);
SX1278 radio = new Module(Nss, Dio0, Rst, Dio1);
volatile bool receiveFlag = false;
void setReceiveFlag() {
  receiveFlag = true;
}

Communication *CommunicattionInstance;
void mqttCallbackStatic(char *topic, byte *payload, unsigned int length) {
  if (CommunicattionInstance)
    CommunicattionInstance->mqttCallback(topic, payload, length);
}

Communication::Communication() {

  CommunicattionInstance = this;
}

void Communication::begin() {
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
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
    while (true) {
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    radio.setPacketReceivedAction(setReceiveFlag);
  }
}
void Communication::sendToNode() {
  while (!isEmpty(buffDataFromDisplay)) {
    msgSendToNode = dequeue(buffDataFromDisplay);
    Serial.println(msgSendToNode);
    transmissionState = radio.transmit(msgSendToNode);
    Serial.println(msgSendToNode);
    if (transmissionState == RADIOLIB_ERR_NONE)
      Serial.println("transmission finished!");
    else {
      Serial.print("failed, code ");
      Serial.println(transmissionState);
    }
    radio.finishTransmit();
    state = radio.startReceive();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}
void Communication::sendToDisplay() {
  while (!isEmpty(buffDataFromNode)) {
    msgSendToDisplay = dequeue(buffDataFromNode);
    Serial2.print(msgSendToDisplay);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void Communication::receiveFromNode() {
  if (receiveFlag) {
    receiveFlag = false;
    state = radio.readData(msgFromNode);
    Serial.println(msgFromNode);
    if (state == RADIOLIB_ERR_NONE) {
      if (!isFull(buffDataFromNode)) {
        enqueueData(buffDataFromNode, msgFromNode.c_str());
      }
    }
  }
}
void Communication::receiveFromDisplay() {
  if (Serial2.available()) {
    msgFromDisplay = Serial2.readString();
    Serial.println(msgFromDisplay);
    if (!isFull(buffDataFromDisplay)) {
      enqueueData(buffDataFromDisplay, msgFromDisplay.c_str());
    }
  }
}

void Communication::connnectToWifi(String user, String pass) {
  Serial.print("Connecting to WiFi");
  WiFi.begin(user, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void Communication::connnectToMQTT(String server, String topicSubscribe) {
  client.setServer(server.c_str(), mqttPort);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client12", mqttUser.c_str(), mqttPassword.c_str())) {
      Serial.println("connected");
      client.subscribe(topicSubscribe.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 second");
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
  client.setCallback(mqttCallbackStatic);
}
void Communication::sendToMQTT(String topic, String msg) {
  while (!isEmpty(buffDataToMqtt)) {
    if (client.connected()) {
      msgToMQTT = dequeue(buffDataToMqtt);
      client.publish(topic.c_str(), msgToMQTT.c_str());
    } else {
      Serial.println("MQTT not connected");
    }
  }
}

void Communication::mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  if (strcmp(topic, mqttTopicSubscribe.c_str()) == 0) {
    msgFromMQTT = "";
    for (unsigned int i = 0; i < length; i++) {
      msgFromMQTT += (char)payload[i];
    }
    Serial.println(msgFromMQTT);
    if (!isFull(buffDataFromMqtt)) {
      enqueueData(buffDataFromMqtt, msgFromMQTT.c_str());
    }
  }
}

void Communication::mqttReconnect(String topicSubscribe) {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client12", mqttUser.c_str(), mqttPassword.c_str())) {
      Serial.println("connected");
      client.subscribe(topicSubscribe.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void Communication::processMqtt() {
  client.loop();
}
void Communication::analizeDataFromMQTT() {
  while (!isEmpty(buffDataFromMqtt)) {
    buffermsgFromMQTT = dequeue(buffDataFromMqtt);

    // Deserialize JSON from MQTT message
    DeserializationError error = deserializeJson(doc, buffermsgFromMQTT);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      continue;  // Skip to next message if parsing failed
    }

    String msgLora = "";
    serializeJson(doc, msgLora);

    if (msgLora.length() > MAX_PACKET_SIZE) {
      Serial.println("msgLora too long: ");
      Serial.println(msgLora);
    } else {
      if (!isFull(buffDataFromDisplay)) {
        enqueueData(buffDataFromDisplay, msgLora.c_str());
        Serial.println("send to Node: ");
      }
    }

    doc.clear();  // Clear for next message
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
