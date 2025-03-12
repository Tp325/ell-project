#ifndef SensorLib_h
#define SensorLib_h
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "config.h"
class RS485Sensor {
private:
  SoftwareSerial *mySerial;
  byte receiveData[8];
  byte sendData[8];
  unsigned short crc;
  unsigned short bufferCrc;
  unsigned char crcHigh;
  unsigned char crcLow;
public:
  RS485Sensor(int RX = RXPin, int TX = TXPin);
  unsigned short calculateCRC(unsigned char *data, unsigned short length);
  void begin();
  int getSensorValue(byte IDOfSensor);
  int getDistance(byte IDOfSensor);
};
#endif
