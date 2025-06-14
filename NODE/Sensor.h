#ifndef Sensor_h
#define Sensor_h
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "config.h"
class RS485Sensor {
private:
  int getSensorValue(byte IDOfSensor);
  byte receiveData[8];
  byte sendData[8];
  unsigned short crc;
  unsigned short bufferCrc;
  unsigned char crcHigh;
  unsigned char crcLow;
  int buffSensorValue=0;
public:
  RS485Sensor();
  unsigned short calculateCRC(unsigned char *data, unsigned short length);
  void begin();
  float getDistance(byte IDOfSensor, float SensorpieLenght);
};
#endif
