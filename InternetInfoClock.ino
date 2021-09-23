#include "RTClib.h"
#include "EEPROMex.h"
#include <Adafruit_NeoPixel.h>

RTC_DS3231 rtc;

uint32_t timeTimer,


#define PIN        6
#define NUMPIXELS 30  //4 цифры по 7 сегментов = 28, плюс 2 точки = 30 шт.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
const uint8_t digitHEX[] = {0x3f, 0x06, 0x5b, 0x4f,
	0x66, 0x6d, 0x7d, 0x07,
	0x7f, 0x6f, 0x00, 0x40
};//0~9, ,-

uint8_t digToHEX(uint8_t digit) {
	return digitHEX[digit];
}

#define _A 0x77
#define _B 0x7f
#define _C 0x39
#define _D 0x3f
#define _E 0x79
#define _F 0x71
#define _G 0x3d
#define _H 0x76
#define _J 0x1e
#define _L 0x38
#define _N 0x37
#define _O 0x3f
#define _P 0x73
#define _S 0x6d
#define _U 0x3e
#define _Y 0x6e
#define _a 0x5f
#define _b 0x7c
#define _c 0x58
#define _d 0x5e
#define _e 0x7b
#define _f 0x71
#define _h 0x74
#define _i 0x10
#define _j 0x0e
#define _l 0x06
#define _n 0x54
#define _o 0x5c
#define _q 0x67
#define _r 0x50
#define _t 0x78
#define _u 0x1c
#define _y 0x6e
#define _dash 0x40
#define _under 0x08
#define _equal 0x48
#define _empty 0x00
#define _degree 0x63

#define _0 0x3f
#define _1 0x06
#define _2 0x5b
#define _3 0x4f
#define _4 0x66
#define _5 0x6d
#define _6 0x7d
#define _7 0x07
#define _8 0x7f
#define _9 0x6f

void clear(void)
{
	display(0x00, 0x7f);
	display(0x01, 0x7f);
	display(0x02, 0x7f);
	display(0x03, 0x7f);
	lastData[0] = 0x00;
	lastData[1] = 0x00;
	lastData[2] = 0x00;
	lastData[3] = 0x00;
}

void display(uint8_t DispData[])
{
	uint8_t SegData[4];
	for (byte i = 0; i < 4; i ++) {
		//if (DispData[i] == 0x7f) SegData[i] = 0x00;
		//else 
		//{
		lastData[i] = digitHEX[DispData[i]];
		SegData[i] = digitHEX[DispData[i]] + PointData;
		//}
	}
	sendArray(SegData);
}

void displayClock(uint8_t hrs, uint8_t mins) {
	if (hrs > 99 || mins > 99) return;
	uint8_t disp_time[4];	
	if ((hrs / 10) == 0) disp_time[0] = 10;
	else disp_time[0] = (hrs / 10);
	disp_time[1] = hrs % 10;
	disp_time[2] = mins / 10;
	disp_time[3] = mins % 10;
	display(disp_time);
}


void displayInt(int value) {
	if (value > 9999 || value < -999) return;
	boolean negative = false;
	boolean neg_flag = false;
	byte digits[4];
	if (value < 0) negative = true;	
	value = abs(value);	
	digits[0] = (int)value / 1000;      	// количесто тысяч в числе
	uint16_t b = (int)digits[0] * 1000; 	// вспомогательная переменная
	digits[1] = ((int)value - b) / 100; 	// получем количество сотен
	b += digits[1] * 100;               	// суммируем сотни и тысячи
	digits[2] = (int)(value - b) / 10;  	// получем десятки
	b += digits[2] * 10;                	// сумма тысяч, сотен и десятков
	digits[3] = value - b;              	// получаем количество единиц
	
	if (!negative) {
		for (byte i = 0; i < 3; i++) {
			if (digits[i] == 0) digits[i] = 10;
			else break;
		}
	} else {
		for (byte i = 0; i < 3; i++) {
			if (digits[i] == 0) {
				if (digits[i + 1] == 0){
					digits[i] = 10;
				} else {
					digits[i] = 11;
					break;
				}
			}			
		}
	}
	display(digits);
}

void brightness(uint8_t brightness, uint8_t SetData, uint8_t SetAddr)
{
	Установить константу яркости
}

void display(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3) {
	uint8_t dispArray[] = {bit0, bit1, bit2, bit3};	
	display(dispArray);
}


void sendArray(uint8_t sendData[]) {
  clear();
  pixels.clear();
  
	
  /**
  * Особенности отображение:
  * позиция 0..6   - первая цифра
  * позиция 7..13  - вторая цифра
  * позиция 14..15 - первая и вторая точки разделители часов и минут
  * позиция 16..22 - третья цифра
  * позиция 23..29 - четвертая цифра
  *
  *Т.е. базовые координаты "B" :
     0  - первая цифра
     7  - вторая цифра
     16 - третья цифра
     23 - четвертая цифра
     
       a
     f   b
       g
     e   c
       d
     
     B+0 - сегмент a (верхний горизонтальный)
     B+1 - сегмент f (левый верхний вертикальный)
     B+2 - сегмент g (средний горизонтальный)
     B+3 - сегмент e (левый нижний вертикальный)
     B+4 - сегмент d (нижний горизонтальный)
     B+5 - сегмент c (правый нижний вертикальный)
     B+6 - сегмент b (правый верхний вертикальный)
  */
	for (byte i = 0; i < 4; i ++) {
		sendData[i]
   pixels.setPixelColor(i, pixels.Color(0, 150, 0) + констаната яркости );
   
	}
pixels.show();
}

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
  
  //Инициализация RGB-ленты
  pixels.begin();
  clear();
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
