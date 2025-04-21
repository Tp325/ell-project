#include "Storage.h"
Storage::Storage() {}
void Storage::begin() {
  EEPROM.begin((numberOfPool * 12) + 16);
  vTaskDelay(200 / portTICK_PERIOD_MS);
  for (int i = 1; i <= numberOfPool; i++) {
    pool[i].maxValue = readFromEEPROM(pool[i].adrressOfSavedDataInEEPROM);
    pool[i].midValue = readFromEEPROM(pool[i].adrressOfSavedDataInEEPROM + sizeof(float));
    pool[i].minValue = readFromEEPROM(pool[i].adrressOfSavedDataInEEPROM + (2 * sizeof(float)));
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
      EEPROM.put(pool[IDOfPool].adrressOfSavedDataInEEPROM, pool[IDOfPool].maxValue);
      EEPROM.put(pool[IDOfPool].adrressOfSavedDataInEEPROM + sizeof(float), pool[IDOfPool].midValue);
      EEPROM.put(pool[IDOfPool].adrressOfSavedDataInEEPROM + (2 * sizeof(float)), pool[IDOfPool].minValue);
      EEPROM.commit();
    }
  }

}
float Storage::readFromEEPROM(int adrress) {
  EEPROM.get(adrress, buffReadData);
  return buffReadData;
}