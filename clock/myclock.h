#ifndef MYCLOCK_H
#define MYCLOCK_H

#include "Adafruit_GFX.h"   // Core graphics library
#include "RGBmatrixPanel_my.h" // Hardware-specific library
#include "fontTiny.h"
#include "fontSmall.h"
#include "font3.h"
#include "fontBig.h"
#include "small_digits.h"
#include "russian.h"
#include <Time.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

#define SET_HOUR 200
#define SET_MIN 201
#define SET_DAY 202
#define SET_MONTH 203
#define SET_YEAR 204

// scrolltext demo for Adafruit RGBmatrixPanel library.
// Demonstrates double-buffered animation on our 16x32 RGB LED matrix:
// http://www.adafruit.com/products/420

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

const uint8_t PROGMEM CLK=8,  // MUST be on PORTB! (Use pin 11 on Mega)
	LAT=A3,
	OE=9,
	A=A0,
	B=A1,
	C=A2,
	INPUT_CLOCK= 13,
	OUTPUT_CLOCK= 12,
	DATA1= 11,
	DATA2= 10,
	HEADER_BYTE=B11001010,
	NUMOFDADABYTES=12,
	hour_index=0,
	min_index=1,
	sec_index=2,
	day_index=3,
	month_index=4,
	weekday_index=5,
	year_index=6,
	temp_index=7,
	humidity_index=8,
	brightness_index=9,
	screen_mode_index=10,
	error_index=11;

const uint8_t PROGMEM year_offset=2,
		temperature_offset=4,
		date_offset=0,
		humidity_offset=6,
		second_row_y=8;

const char StatusMessage[] PROGMEM  = {"Status"};
const char OkMessage[] PROGMEM  = {"Ok"};
const char ErrorMessage[] PROGMEM  = {"Error"};
class myclock
{
    public:
        myclock();
        void init();
        void update();
		void setlocaltime();
		void print_status();
		
		inline bool is_set_hour() {return current_clock_data[screen_mode_index]==SET_HOUR;}
		inline bool is_set_min() {return current_clock_data[screen_mode_index]==SET_MIN;}
		inline bool is_set_day() {return current_clock_data[screen_mode_index]==SET_DAY;}
		inline bool is_set_month() {return current_clock_data[screen_mode_index]==SET_MONTH;}
		inline bool is_set_year() {return current_clock_data[screen_mode_index]==SET_YEAR;}
		inline bool myblink() {return millis()%1000>500;}
		
		void put_tiny_string_from_progmem(uint8_t x, uint8_t y, const char message[],uint8_t r, uint8_t g,uint8_t b);
		
        void putDigitLarge(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b);
        void putLargeTime(uint8_t x, uint8_t y, int h,int m,int s, uint8_t r, uint8_t g,uint8_t b);
		
		void putSmallChar(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b);
		void putSmallTime(int x, int y, int h,int m,int s, uint8_t r, uint8_t g,uint8_t b);
		
		void putChar(uint8_t x, uint8_t y,const unsigned char * c,uint8_t h,uint8_t w, uint8_t r, uint8_t g,uint8_t b);
		void putRusChar(uint8_t x, uint8_t y, int c, uint8_t r, uint8_t g,uint8_t b);
		void putDigit(uint8_t x, uint8_t y, const unsigned char * c,uint8_t h,uint8_t w, uint8_t r, uint8_t g,uint8_t b);
		
		void tinyTime(uint8_t x, uint8_t y, int h, int m,int s, uint8_t r, uint8_t g,uint8_t b);
		void putCharTiny(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b);
		void putTinyDate(int x, int y, int d,int m,int yr, uint8_t r, uint8_t g,uint8_t b);
		
		void putNormalDigit(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b);
		void putNormalChar(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b);
		void putNormalTime(short int x,short int y, int h,int m,int s, uint8_t r, uint8_t g,uint8_t b);
		
		void putTemp(int x, int y, uint8_t& t, uint8_t r, uint8_t g,uint8_t b);
		void putHumidity(int x, int y, uint8_t& h, uint8_t r, uint8_t g,uint8_t b);
		void putDate(int x, int y, int d,int m, uint8_t r, uint8_t g,uint8_t b);
		void putYear(int x, int y, int a, uint8_t r, uint8_t g,uint8_t b);
		void putTempHum(int x, int y, uint8_t& t,uint8_t& h, uint8_t r, uint8_t g,uint8_t b);
		void shiftTemp();
		
		void update_local_time();
		void set_time();
		void set_date();
		void set_year();
		
		void clock_type1();
		void clock_type2();
		void clock_type3();
		
		void receive_data();
		inline void push_bit(uint8_t in_bit);
		uint8_t data_ready=0;
    protected:
    private:
		RGBmatrixPanel_my matrix;
		uint8_t out_clk_state=0,
			bit_counter=0,
			byte_counter=0,
			sync_ok=0,
			current_byte=0;
		uint8_t
			first_shift_done=0,
			shift_in_progress=0,
			shift_offset=0,
			last_shift_sec=0,
			clock1_state=0,
			system_time_set=0;
		uint8_t	r=0,g=1,b=0;
		tmElements_t tm;
		//Clock data array
		/* 0 - hour
		1- min
		2-sec
		3-day
		4-month
		5-weekday
		6-last 2 digits of year
		7-temperature
		8-humidity
		9-light
		10 - buttons
		11- resetved
		*/
		uint8_t	current_clock_data[12];
};

#endif // MYCLOCK_H
