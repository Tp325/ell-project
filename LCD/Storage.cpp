#include "Storage.h"
Storage::Storage() {}
void Storage::begin() {
  EEPROM.begin((numberOfPool * 12) + 16);
  vTaskDelay(200 / portTICK_PERIOD_MS);
  for (int i = 1; i <= numberOfPool; i++) {
    pool[i].maxValue = readFromEEPROM(pool[i].addressOfSavedDataInEEPROM);
    pool[i].midValue = readFromEEPROM(pool[i].addressOfSavedDataInEEPROM + sizeof(float));
    pool[i].minValue = readFromEEPROM(pool[i].addressOfSavedDataInEEPROM + (2 * sizeof(float)));
    if (isnan(pool[i].maxValue))
      pool[i].maxValue = 0.0;
    if (isnan(pool[i].midValue))
      pool[i].midValue = 0.0;
    if (isnan(pool[i].minValue))
      pool[i].minValue = 0.0;
  }
}
void Storage::saveToEEPROM() {
  while (!isEmpty(buffDataToEEPROM)) {
    IDOfPool = atoi(dequeue(buffDataToEEPROM));
    if (IDOfPool > 0 && IDOfPool <= numberOfPool) {
      maxValueInEEPROM = readFromEEPROM(pool[IDOfPool].addressOfSavedDataInEEPROM);
      midValueInEEPROM = readFromEEPROM(pool[IDOfPool].addressOfSavedDataInEEPROM + sizeof(float));
      minValueInEEPROM = readFromEEPROM(pool[IDOfPool].addressOfSavedDataInEEPROM + 2 * sizeof(float));
      if (fabs(maxValueInEEPROM - pool[IDOfPool].maxValue) < 0.1) {
        EEPROM.put(pool[IDOfPool].addressOfSavedDataInEEPROM, pool[IDOfPool].maxValue);
        needCommit = true;
      }
      if (fabs(midValueInEEPROM - pool[IDOfPool].midValue) < 0.1) {
        EEPROM.put(pool[IDOfPool].addressOfSavedDataInEEPROM + sizeof(float), pool[IDOfPool].midValue);
        needCommit = true;
      }
      if (fabs(minValueInEEPROM - pool[IDOfPool].minValue) < 0.1) {
        EEPROM.put(pool[IDOfPool].addressOfSavedDataInEEPROM + (2 * sizeof(float)), pool[IDOfPool].minValue);
        needCommit = true;
      }
      if (needCommit) {
        EEPROM.commit();
        needCommit = false;
      }
    }
  }
}
float Storage::readFromEEPROM(int adrress) {
  EEPROM.get(adrress, buffReadData);
  return buffReadData;
}