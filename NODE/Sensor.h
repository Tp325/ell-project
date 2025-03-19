#ifndef Sensor_h
#define Sensor_h
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "config.h"
class RS485Sensor {
private:
  byte receiveData[8];
  byte sendData[8];
  unsigned short crc;
  unsigned short bufferCrc;
  unsigned char crcHigh;
  unsigned char crcLow;
public:
  RS485Sensor();
  int getSensorValue(byte IDOfSensor);
  unsigned short calculateCRC(unsigned char *data, unsigned short length);
  void begin();
  int getDistance(byte IDOfSensor);
};
#endif
