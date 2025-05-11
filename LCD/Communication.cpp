#include "Communication.h"
DynamicJsonDocument doc(1024);
DynamicJsonDocument doc1(1024);

Communication::Communication() {
}
void Communication::begin() {
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  uart_driver_install(UART_NUM_2, 1024, 1024, 0, NULL, 0);
}
void Communication::sendToSink() {
  while (!isEmpty(buffDataToSink)) {
    Serial2.println(dequeue(buffDataToSink));
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}
void Communication::receiveFromSink() {
  if (Serial2.available()) {
    msgFromSink = Serial2.readStringUntil('\n');
    if (!isFull(buffDataFromSink)) {
      enqueueData(buffDataFromSink, msgFromSink.c_str());
    }
  }
}
void Communication::analizeDataToSink() {
  if (command != "") {
    if (command == "GD") {
      for (int i = 1; i <= numberOfPool; i++) {
        doc.clear();
        doc["is"] = 0;
        doc["SID"] = StationID;
        doc["ID"] = i;
        doc["a"] = uint8_t(pool[i].autoStatus);
        doc["ma"] = pool[i].maxValue;
        doc["md"] = pool[i].midValue;
        doc["mn"] = pool[i].minValue;
        msgToSink = "";
        serializeJson(doc, msgToSink);
        if (!isFull(buffDataToSink)) {
          enqueueData(buffDataToSink, msgToSink.c_str());
        }
        doc.clear();
        doc["is"] = 0;
        doc["SID"] = StationID;
        doc["ID"] = i;
        doc["i"] = uint8_t(pool[i].inStatus);
        doc["o"] = uint8_t(pool[i].outStatus);
        msgToSink = "";
        serializeJson(doc, msgToSink);
        if (!isFull(buffDataToSink)) {
          enqueueData(buffDataToSink, msgToSink.c_str());
        }
        doc.clear();
        doc["is"] = 0;
        doc["SID"] = StationID;
        doc["ID"] = i;
        doc["mucn"] = pool[i].mucnuoc;
        msgToSink = "";
        serializeJson(doc, msgToSink);
        if (!isFull(buffDataToSink)) {
          enqueueData(buffDataToSink, msgToSink.c_str());
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
      }
      command = "";
    }
  } else {
    doc.clear();
    doc["is"] = 0;  //is send by server
    doc["SID"] = StationID;
    if (pool[lastPage != 0 ? lastPage : currentPage].autoStatus == 2 || pool[lastPage != 0 ? lastPage : currentPage].autoStatus == 3) {
      doc["ID"] = pool[lastPage != 0 ? lastPage : currentPage].poolID;
      doc["a"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].autoStatus);
      doc["ma"] = pool[lastPage != 0 ? lastPage : currentPage].maxValue;  // vì trong setting page giá trị current page =0 nên lưu vào biến last page là page hiện tại
      doc["md"] = pool[lastPage != 0 ? lastPage : currentPage].midValue;
      doc["mn"] = pool[lastPage != 0 ? lastPage : currentPage].minValue;
    } else if (isValStatusButtonPressed == 1) {
      doc["ID"] = pool[lastPage != 0 ? lastPage : currentPage].poolID;
      doc["i"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].inStatus);
      doc["o"] = uint8_t(pool[lastPage != 0 ? lastPage : currentPage].outStatus);
    } else if (isSettingValChange == 2) {
      doc["ID"] = pool[lastPage != 0 ? lastPage : currentPage].poolID;
      doc["ma"] = pool[lastPage != 0 ? lastPage : currentPage].maxValue;  // vì trong setting page giá trị current page =0 nên lưu vào biến last page là page hiện tại
      doc["md"] = pool[lastPage != 0 ? lastPage : currentPage].midValue;
      doc["mn"] = pool[lastPage != 0 ? lastPage : currentPage].minValue;
    }
    serializeJson(doc, msgToSink);
    if (!isFull(buffDataToSink)) {
      enqueueData(buffDataToSink, msgToSink.c_str());
    }
  }
}

void Communication::analizeData() {
  while (!isEmpty(buffDataFromSink)) {
    bufferMsgFromSink = dequeue(buffDataFromSink);
    deserializeJson(doc1, bufferMsgFromSink);
    if (doc1.containsKey("SID")) {
      if (doc1["SID"].as<String>() == StationID) {
        if (doc1.containsKey("ID")) {
          IDOfPool = doc1["ID"].as<int>();
          pool[IDOfPool].poolID = IDOfPool;
        }
        if (doc1.containsKey("mucn")) {
          pool[IDOfPool].mucnuoc = doc1["mucn"].as<float>();
        }
        if (doc1["is"].as<int>() == 0) {
          if (doc1.containsKey("i")) {
            pool[IDOfPool].inStatus = doc1["i"].as<uint8_t>();
          }
          if (doc1.containsKey("o")) {
            pool[IDOfPool].outStatus = doc1["o"].as<uint8_t>();
          }
          if (doc1.containsKey("a")) {
            pool[IDOfPool].autoStatus = doc1["a"].as<uint8_t>();
            pool[IDOfPool].inStatus = 0;
            pool[IDOfPool].outStatus = 0;
          }
        } else if (doc1["is"].as<int>() == 1) {
          if (doc1.containsKey("cm")) {
            command = doc1["cm"].as<String>();
          }
          if (doc1.containsKey("i")) {
            pool[IDOfPool].inStatus = doc1["i"].as<uint8_t>();
          }
          if (doc1.containsKey("o")) {
            pool[IDOfPool].outStatus = doc1["o"].as<uint8_t>();
          }
          if (doc1.containsKey("a")) {
            pool[IDOfPool].inStatus = 0;
            pool[IDOfPool].outStatus = 0;
            pool[IDOfPool].autoStatus = doc1["a"].as<uint8_t>();
          }
          if (doc1.containsKey("ma")) {
            if (pool[IDOfPool].maxValue != doc1["ma"].as<float>()) {
              pool[IDOfPool].maxValue = doc1["ma"].as<float>();
              haveDifferentValue = 1;
            }
            if (pool[IDOfPool].midValue != doc1["md"].as<float>()) {
              pool[IDOfPool].midValue = doc1["md"].as<float>();
              haveDifferentValue = 1;
            }
            if (pool[IDOfPool].minValue != doc1["mn"].as<float>()) {
              pool[IDOfPool].minValue = doc1["mn"].as<float>();
              haveDifferentValue = 1;
            }
          }
        }
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
