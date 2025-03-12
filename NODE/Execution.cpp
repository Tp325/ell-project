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
  // bufferDistance[poolID] = getDistance(distanceSensor[1]);
}
