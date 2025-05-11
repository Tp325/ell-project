#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3);
unsigned char data[8];
String dataReceive;
char buffer[8] = {};
int a = 0;
unsigned short crc;
unsigned short bufferCrc;
unsigned char crcHigh;
unsigned char crcLow;
unsigned short calculateCRC(unsigned char *data, unsigned short length) {
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
void analize() {
  bufferCrc = calculateCRC(data, 6);
  data[6] = bufferCrc & 0xFF;
  data[7] = (bufferCrc >> 8) & 0xFF;
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(115200);
}

void loop() {
  if (mySerial.available() >= 8) {
    mySerial.readBytes(buffer, 8);
    for (int i = 0; i < 8; i++) {
      if ((byte)buffer[i] < 0x10) Serial.print("0");
      Serial.print((byte)buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
  }
  if (Serial.available()) {
    dataReceive = Serial.readString();
    data[a] = (char)strtol(dataReceive.c_str(), NULL, 16);
    Serial.print(data[a], HEX);
    Serial.print(" ");
    a++;
  }
  if (a == 6) {
    Serial.println(" ");
    analize();
    for (int i = 0; i < 8; i++) {
      Serial.print(data[i], HEX);
      Serial.print(" ");
    }
    Serial.println(" ");
    mySerial.write(data, 8);
    a = 0;
  }
}
