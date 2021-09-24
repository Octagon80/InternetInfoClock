#include <Adafruit_NeoPixel.h>
#define IS_BIT_SET(n,x)   (((n & (1 << x)) != 0) ? 1 : 0)

#define PIN        6
#define NUMPIXELS 30  //4 цифры по 7 сегментов = 28, плюс 2 точки = 30 шт.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


/*
#define _A 0x77
#define _B 0x7f
#define _C 0b00011001
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

#define _under 0x08
#define _equal 0x48
*/
#define _0 0b01111011
#define _1 0b01100000
#define _2 0b01011101
#define _3 0b00111101
#define _4 0b01100101
#define _5 0b00111111
#define _6 0b00111111
#define _7 0b01100001
#define _8 0b01111111
#define _9 0b01110111
#define _empty 0b00000000
#define _degree 0b00011011
#define _dash  0b00000100


const uint8_t digitHEX[] = {_0, _1, _2, _3,
	_4, _5, _6, _7,
	_8, _9, _empty, _dash
};//0~9, ,-



uint8_t digToHEX(uint8_t digit) {
	return digitHEX[digit];
}


//Яркость отображения в %
uint8_t _brightness = 0;
//Цветовая палитра отображения (0..255)
uint8_t _r = 10;
uint8_t _g = 10;
uint8_t _b = 10;



void clear(void)
{
	display(0x00, _empty);
	display(0x01, _empty);
	display(0x02, _empty);
	display(0x03, _empty);
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


//Яркость в %
void brightness(uint8_t brightness, uint8_t SetData, uint8_t SetAddr)
{
	//Установить константу яркости
  _brightness = brightness;
}

void display(uint8_t bit0, uint8_t bit1, uint8_t bit2, uint8_t bit3) {
	uint8_t dispArray[] = {bit0, bit1, bit2, bit3};
	display(dispArray);
}


//sendData-4 битовых масок отображаемых символов (см. define)
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
  uint8_t ledOffsets = [0,7,16,23];
  
	for (uint8_t i = 0; i < 4; i ++) {
    //_brightness
    uint8_t r;
    uint8_t g;
    uint8_t b;

    //на символ 7 сегментов => 7 бит
    for( uint8_t nbit=0; nbit<7; nbit++){
       if( IS_BIT_SET(sendData[i], nbit ) ){
         r = (_r * _brightness) / 100;
         g = (_g * _brightness) / 100;
         b = (_b * _brightness) / 100;
       }else{
          //выключен сегмент
          r = 0; g = 0; b = 0;
       }
       pixels.setPixelColor(ledOffsets[i] + nbit, pixels.Color(r, g, b) );
    }
	}
pixels.show();
}




void setup() {
  //Инициализация RGB-ленты
  pixels.begin();
  clear();

   display(_1, _2, _3, _4);
  }


  void loop() {
  
  }

