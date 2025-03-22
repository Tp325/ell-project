#include "Execution.h"
Execution::Execution() {
}
void Execution::begin() {
  for (int i = 1; i <= numberOfPool; i++) {
    pool[i].inStatus = 0;
    pool[i].outStatus = 0;
    pinMode(pool[i].supplyPin, OUTPUT);
    pinMode(pool[i].drainPin, OUTPUT);
  }
  for (int i = 1; i <= numberOfPool; i++) {
    digitalWrite(pool[i].supplyPin, LOW);
    digitalWrite(pool[i].drainPin, LOW);
  }
}
void Execution::drainOut(int poolID) {
  digitalWrite(pool[poolID].drainPin, HIGH);
}
void Execution::supplyIn(int poolID) {
  digitalWrite(pool[poolID].supplyPin, HIGH);
}
void Execution::xdrainOut(int poolID) {
  digitalWrite(pool[poolID].drainPin, LOW);
}
void Execution::xsupplyIn(int poolID) {
  digitalWrite(pool[poolID].supplyPin, LOW);
}
void Execution::autoRun(int poolID) {
  // Serial.println("******************");
  // Serial.printf("ID:%d\n", poolID);
  // Serial.println(pool[poolID].stepOfAuto);
  // Serial.println(pool[poolID].mucnuoc);
  if (pool[poolID].maxValue > pool[poolID].midValue && pool[poolID].midValue > pool[poolID].minValue) {
    if (pool[poolID].stepOfAuto == 0) {
      if (pool[poolID].mucnuoc <= pool[poolID].minValue)
        pool[poolID].stepOfAuto++;
      else
        digitalWrite(pool[poolID].drainPin, HIGH);
    } else if (pool[poolID].stepOfAuto == 1) {
      if (pool[poolID].mucnuoc >= pool[poolID].maxValue)
        pool[poolID].stepOfAuto++;
      else {
        digitalWrite(pool[poolID].drainPin, LOW);
        digitalWrite(pool[poolID].supplyPin, HIGH);
      }
    } else if (pool[poolID].stepOfAuto == 2) {
      if (pool[poolID].mucnuoc <= pool[poolID].midValue) {
        pool[poolID].stepOfAuto++;
      } else {
        digitalWrite(pool[poolID].drainPin, HIGH);
        digitalWrite(pool[poolID].supplyPin, LOW);
      }
    } else if (pool[poolID].stepOfAuto == 3) {
      if (pool[poolID].mucnuoc >= pool[poolID].maxValue) {
        pool[poolID].stepOfAuto++;
        pool[poolID].isDoneAutoMode = 1;
        pool[poolID].autoStatus = 0;
      } else {
        digitalWrite(pool[poolID].drainPin, LOW);
        digitalWrite(pool[poolID].supplyPin, HIGH);
      }
    } else {
      // Serial.printf("end of auto ID:%d \n", poolID);
      pool[poolID].autoStatus = 0;
    }
  } else pool[poolID].autoStatus = 0;
}
