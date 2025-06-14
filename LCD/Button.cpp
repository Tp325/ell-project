#include "Button.h"
Button::Button() {}
void Button::begin() {
  pinMode(up, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(Left, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(select, INPUT_PULLUP);
  pinMode(supplyval, INPUT_PULLUP);
  pinMode(drainval, INPUT_PULLUP);
  pinMode(settingButton, INPUT_PULLUP);
  pinMode(autoButton, INPUT_PULLUP);
}

void Button::checkState() {
  //*************Page setting status***********
  if (digitalRead(settingButton) == 0) {
    if (currentPage == 0) {
      currentPage = lastPage;
      lastPage = 0;
    } else {
      lastPage = currentPage;
      pointerPossitionAtSettingPage = 5;
      isChangingValueAtSettingPage = 0;
      currentPage = 0;
    }
    isPageChangeToSettingPage = 1;
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  //*******************************
  if (currentPage != 0) {
    if (digitalRead(Right) == 0 && currentPage <= numberOfPool) {
      currentPage++;
      if (currentPage == numberOfPool + 1)
        currentPage = 1;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (digitalRead(Left) == 0 && currentPage <= numberOfPool) {
      currentPage--;
      if (currentPage == 0)
        currentPage = numberOfPool;
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    if (digitalRead(autoButton) == 0) {
      if (pool[currentPage].maxValue > pool[currentPage].midValue && pool[currentPage].midValue > pool[currentPage].minValue && int(pool[currentPage].mucnuoc) != 404) {
        if (pool[currentPage].autoStatus < 2)
          pool[currentPage].autoStatus = pool[currentPage].autoStatus == 0 ? 3 : 2;
        pool[currentPage].outStatus = 0;
        pool[currentPage].inStatus = 0;
        isValStatusButtonPressed = 1;
        vTaskDelay(500 / portTICK_PERIOD_MS);
      } else {
        pool[currentPage].autoStatus = 4;
      }
    }
    if (pool[currentPage].autoStatus == 0) {
      if (digitalRead(supplyval) == 0) {
        if (pool[currentPage].inStatus < 2)
          pool[currentPage].inStatus = pool[currentPage].inStatus == 0 ? 3 : 2;
        isValStatusButtonPressed = 1;
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      if (digitalRead(drainval) == 0) {
        if (pool[currentPage].outStatus < 2)
          pool[currentPage].outStatus = pool[currentPage].outStatus == 0 ? 3 : 2;
        isValStatusButtonPressed = 1;
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
    }
  }
  //*************Page setting status********************
  else if (currentPage == 0) {
    // number 15 and 5 is the maximum and smallest row of text display in lcd at setting page
    if (digitalRead(select) == 0) {
      isChangingValueAtSettingPage = !isChangingValueAtSettingPage;
      isSettingValChange++;
      vTaskDelay(250 / portTICK_PERIOD_MS);
    }
    if (isChangingValueAtSettingPage == 0) {
      if (digitalRead(up) == 0 && pointerPossitionAtSettingPage >= 5) {
        pointerPossitionAtSettingPage -= 3;
        if (pointerPossitionAtSettingPage < 5)
          pointerPossitionAtSettingPage = 11;
        vTaskDelay(250 / portTICK_PERIOD_MS);
      }
      if (digitalRead(down) == 0 && pointerPossitionAtSettingPage <= 11) {
        pointerPossitionAtSettingPage += 3;
        if (pointerPossitionAtSettingPage > 11)
          pointerPossitionAtSettingPage = 5;
        vTaskDelay(250 / portTICK_PERIOD_MS);
      }
    } else if (isChangingValueAtSettingPage == 1) {
      if (digitalRead(up) == 0) {
        if (pointerPossitionAtSettingPage == 5) {
          pool[lastPage].maxValue += 0.5;
        }
        if (pointerPossitionAtSettingPage == 8) {
          pool[lastPage].midValue += 0.5;
        }
        if (pointerPossitionAtSettingPage == 11) {
          pool[lastPage].minValue += 0.5;
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
      }
      if (digitalRead(down) == 0) {
        if (pointerPossitionAtSettingPage == 5 && pool[lastPage].maxValue > 0) {
          pool[lastPage].maxValue -= 0.5;
        }
        if (pointerPossitionAtSettingPage == 8 && pool[lastPage].midValue > 0) {
          pool[lastPage].midValue -= 0.5;
        }
        if (pointerPossitionAtSettingPage == 11 && pool[lastPage].minValue > 0) {
          pool[lastPage].minValue -= 0.5;
        }
        vTaskDelay(250 / portTICK_PERIOD_MS);
      }
      haveDifferentValue = 1;
    }
    //************************************************
  }
}
