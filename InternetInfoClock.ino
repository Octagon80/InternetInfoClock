#include "RTClib.h"
#include "EEPROMex.h"

RTC_DS3231 rtc;

uint32_t timeTimer,


void setup() {
// проверка RTC
  if (rtc.begin()) {
    Serial.println("RTC OK");
    clock_state = true;
  } else {
    Serial.println("RTC ERROR");
    clock_state = false;
  }
  
  
  // первый запуск
  if (EEPROM.read(100) != 10) {   // храним какое то число как индикатор запуска
    Serial.println("first start");
    EEPROM.writeByte(100, 10);
    for (byte i = 0; i < 28; i++) {
      EEPROM.writeInt(i * 2, STOCK_UP);
      EEPROM.writeByte(90, 15);
    }
  }
  
  //Вспоминаем что-то
  = EEPROM.read(90); 
   timeTimer = 60000;    // искусственно переполнить таймер времени,
  }
  
  void loop() {
  timeTick();       // таймер времени (по флагу clock_state)
  }
  
  
  void timeTick() {
  if (clock_state) {
    if (millis() - timeTimer > 60000) {   // отсчитываем минуту
      timeTimer = millis();
      // каждую минуту берём время с RTC тип

      DateTime now = rtc.now();
      Serial.print("New minute. Time is ");
      Serial.print(now.hour()); Serial.print(":"); Serial.println(now.minute());

      // забиваем массив для отображения
      uint8_t indicators[4];
      indicators[0] = now.hour() / 10;    // десятки часов
      indicators[1] = now.hour() % 10;    // единицы часов
      indicators[2] = now.minute() / 10;  // десятки минут
      indicators[3] = now.minute() % 10;  // единицы минут

      turnFlag = true;            // разрешить поворот серв
      byte servoNum = 0;
      for (byte i = 0; i < 4; i++) {      // для 4 индикаторов
        for (byte j = 0; j < 7; j++) {    // каждый сегмент
          boolean thisBit;
          // thisBit принимает 0 или 1 - поднять или опустить сегмент.
          // HEXnumbers разбивается на биты, например 0000110 это цифра 1
          // и побитно принимается решение, поднять или опустить сегмент
          thisBit = HEXnumbers[indicators[i]] & (1 << j);
          if (i == 0 && indicators[0] == 0) thisBit = 0;          // не показывать ноль в десятках часов
          if (thisBit) new_angle[servoNum] = up_angle[servoNum];  // поднять
          else new_angle[servoNum] = SERVOMIN;                    // опустить
          servoNum++;                                             // перейти к следующей серво
        }
      }
    }
  }
}
