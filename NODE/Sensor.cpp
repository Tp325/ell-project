#include "Sensor.h"
RS485Sensor::RS485Sensor() {
}
void RS485Sensor::begin() {
  Serial2.begin(115200, SERIAL_8N1, RXPin, TXPin);
}
unsigned short RS485Sensor::calculateCRC(unsigned char *data, unsigned short length) {
  crc = 0xFFFF;
  for (int i = 0; i < length; i++) {
    crc ^= data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}
int RS485Sensor::getSensorValue(byte IDOfSensor) {
  sendData[0] = IDOfSensor;
  sendData[1] = 0x03;
  sendData[2] = 0x00;
  sendData[3] = 0x34;
  sendData[4] = 0x00;
  sendData[5] = 0x01;
  calculateCRC(sendData, 6);
  sendData[6] = crc & 0xFF;
  sendData[7] = (crc >> 8) & 0xFF;
  Serial2.write(sendData, 8);
  vTaskDelay(200 / portTICK_PERIOD_MS);
  if (Serial2.available() >= 7) {
    Serial2.readBytes(receiveData, 7);
    return receiveData[3] << 8 | receiveData[4];
  } else {
    return 0;
  }
}
int RS485Sensor::getDistance(byte IDOfSensor) {
  return getSensorValue(IDOfSensor);
}