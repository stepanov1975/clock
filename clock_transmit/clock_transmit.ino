#include <Wire.h>
//#include <RTClib.h>
#include <DS1307RTC.h>
#include "SparkFunHTU21D_my.h"
#include <Time.h> 
#include <EEPROM.h>

HTU21D myHumidity;

#define INPUT_CLOCK 7
#define OUTPUT_CLOCK 6
#define DATA1 9
#define DATA2 8
#define MODE_BUTTON A2
#define SET_BUTTON A3
#define PLUS_BUTTON 2
#define MINUS_BUTTON 3
#define LIGHT_SENSOR A0
#define BOUNCE_TIME_BUTTON 300

#define SET_HOUR 200
#define SET_MIN 201
#define SET_DAY 202
#define SET_MONTH 203
#define SET_YEAR 204

//Clock data array
#define D_HEADER 	0 //header
#define D_HOUR 		1
#define D_MIN 		2
#define D_SEC 		3
#define D_DAY		4
#define D_MONTH 	5
#define D_WEEKDAY 	6
#define D_YEAR 		7//last 2 digits of year
#define D_TEMP 		8//Temperature
#define D_HUMID 	9//humidity
#define D_BRIGHT 	10//screen brightness
#define D_MODE 		11//Screen mode
#define D_ERROR		12//Error code
const uint8_t PROGMEM data_bytes=13;//Number of data byres for transmittion
/* 
Error codes
0 - ok
1 - no rtc
2 - unable to get time from rtc
3 - no humidity sensor
*/

int output_clock_state,input_clock_state,data1_val,data2_val;


uint8_t	current_clock_data[data_bytes];
volatile unsigned long lastButtonTime = 0;// last time a button was pushed; used for debouncing

uint8_t out_clk_state=0,
	byte_counter=0,
	sync_ok=0,
	header=B11001010,
	rtc_ok=0,
	rtc_running=0,
	humidity_ok=0,
	hum_needed=1,
	hum_cycle=0;
short int bit_counter=7;
short unsigned int numc=0;
float last_temp;
const char *monthName[12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
tmElements_t tm;

void setup() {
	pinMode(INPUT_CLOCK, INPUT);
	pinMode(DATA1, OUTPUT);
	pinMode(DATA2, OUTPUT);
	pinMode(OUTPUT_CLOCK, OUTPUT);
	
	pinMode(MODE_BUTTON, INPUT);
	pinMode(SET_BUTTON, INPUT);
	pinMode(PLUS_BUTTON, INPUT);
	pinMode(MINUS_BUTTON, INPUT);
	
	digitalWrite(DATA1,LOW);
	digitalWrite(DATA2,LOW);
	
	digitalWrite(OUTPUT_CLOCK,LOW);
	output_clock_state=LOW;
	current_clock_data[D_HEADER]=header;
	current_clock_data[D_HOUR]=0;
	current_clock_data[D_MIN]=0;
	current_clock_data[D_SEC]=0;
	current_clock_data[D_DAY]=0;
	current_clock_data[D_MONTH]=0;
	current_clock_data[D_WEEKDAY]=0;
	current_clock_data[D_YEAR]=0;
	current_clock_data[D_TEMP]=0;
	current_clock_data[D_HUMID]=0;
	current_clock_data[D_BRIGHT]=1;
	current_clock_data[D_MODE]=0;
	current_clock_data[D_ERROR]=0;
	
	Serial.begin(9600);
	//Wire.begin();
	
	
	if (RTC.read(tm)) {
		rtc_ok=1;
		Serial.println(F("RTC found"));
		setSyncProvider(RTC.get);   // the function to get the time from the RTC
		if(timeStatus()!= timeSet) {
			Serial.println(F("Unable to sync with the RTC"));
			current_clock_data[D_ERROR]=2;
		}
		else {
			Serial.println(F("RTC has set the system time"));
			rtc_running=1;
		}
	}
	else {
		if (RTC.chipPresent()) {
			Serial.println(F("The DS1307 is stopped.  Please run the SetTime"));
			current_clock_data[D_ERROR]=1;
		} else {
			if (getDate(__DATE__) && getTime(__TIME__)) {
				Serial.println(F("Time parsing ok"));
				// and configure the RTC with this info
				if (RTC.write(tm)) {
					Serial.println(F("TRC time set"));
				}
				else{
					Serial.println(F("DS1307 time set error!  Please check the circuitry."));
				}
			}
		}
	}
	hum_cycle=0;
	myHumidity.begin();
	myHumidity.requestHumidity();
	delay(55);
	float tst_humid=myHumidity.readHumidity();
	myHumidity.requestTemperature();
	delay(55);
	float tst_temp=myHumidity.readTemperature();
	if (tst_humid<998 & tst_temp<998){
		Serial.println(F("Humidity and temperature sensor ok!"));
		humidity_ok=1;
	}
	else{
		humidity_ok=0;
		current_clock_data[D_ERROR]=3;
		if (tst_humid==999) Serial.println(F("Temperature CRC is wrong"));
		if (tst_humid==998) Serial.println(F("Temperature I2C timed out "));
		if (tst_temp==999) Serial.println(F("Humidity CRC is wrong"));
		if (tst_temp==998) Serial.println(F("Humidity  I2C timed out"));
	}
	numc=0;
	get_time();
}

void get_time(){
	//DateTime now = rtc.now();
	current_clock_data[D_HOUR]=hour();
	current_clock_data[D_MIN]=minute();
	current_clock_data[D_SEC]=second();
	current_clock_data[D_DAY]=day();
	current_clock_data[D_MONTH]=month();
	current_clock_data[D_WEEKDAY]=weekday();
	current_clock_data[D_YEAR]=year()-2000;
}

void get_hum(){
	if (humidity_ok==1 & hum_needed==1){
		switch(hum_cycle){
			case 0:
				myHumidity.requestTemperature();
				break;
			case 1:
				last_temp =myHumidity.readTemperature();
				current_clock_data[D_TEMP]=floor(last_temp-1.5);
				break;
			case 2:
				myHumidity.requestHumidity();
				break;
			case 3:
				float hum=myHumidity.readHumidity()+(25.0-last_temp)*-0.15;
				current_clock_data[D_HUMID]=floor(hum+0.5);
				break;
		}
	}
	if (hum_cycle<3) hum_cycle++;
	else hum_cycle=0;
}

void get_light(){
	int light=1024-floor((analogRead(LIGHT_SENSOR)+analogRead(LIGHT_SENSOR)+analogRead(LIGHT_SENSOR)+analogRead(LIGHT_SENSOR))/4+0.5);
	current_clock_data[D_BRIGHT] = map( constrain (light, 0, 1024), 0, 1024, 1, 15); // Get Ambient Light Reading
	//Serial.print(F("Light sensor"));
	//Serial.print(light);
	//Serial.println("");
}

void loop() {
	read_buttons();
	get_light();
	if (current_clock_data[D_MODE]<100) hum_needed=1;
	if (bit_counter<0) {
		bit_counter=7;
		byte_counter++;
		//Serial.println("ok");
		
	}
	if (byte_counter>(data_bytes-1)) {
		byte_counter=0;
		get_time();
		numc++;
		if (numc>4) {
			numc=0;
			get_hum();
		}
	}
	
	input_clock_state=digitalRead(INPUT_CLOCK);
	if (input_clock_state!=output_clock_state) {
		uint8_t current_bit;
		
		current_bit=(current_clock_data[byte_counter]&(1<<bit_counter))>>bit_counter;
		digitalWrite(DATA1,current_bit);
		//Serial.print(current_bit, DEC);
		bit_counter--;
		
		current_bit=(current_clock_data[byte_counter]&(1<<bit_counter))>>bit_counter;
		digitalWrite(DATA2,current_bit);
		//Serial.print(current_bit, DEC);
		bit_counter--;
		
		if (output_clock_state==HIGH) {
			digitalWrite(OUTPUT_CLOCK,LOW);
			output_clock_state=LOW;
		}
		else {
			digitalWrite(OUTPUT_CLOCK,HIGH);
			output_clock_state=HIGH;
		}
	}
}

void read_buttons(){
	if (digitalRead(MODE_BUTTON) == HIGH){
		processModeButton();
	}
	if (digitalRead(SET_BUTTON) == HIGH){
		processSetButton();
	}
	if (digitalRead(PLUS_BUTTON) == HIGH){
		processPlusButton();
		
	}
	if (digitalRead(MINUS_BUTTON) == HIGH){
		processMinusButton();
	}

}

void processModeButton(){
	if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON) return;
	//Serial.println(F("MODE_BUTTON"));
	//current_clock_data[D_MODE]
	/*
	switch(current_clock_data[D_MODE]){
		case 0:
			current_clock_data[D_MODE]=1;
			break;
		case 1:
			current_clock_data[D_MODE]=0;
			break;
		default:
			current_clock_data[D_MODE]=0;
	}
	*/
	lastButtonTime=millis();
	hum_needed=1;
	current_clock_data[D_MODE]++;
	if (current_clock_data[D_MODE]>3) current_clock_data[D_MODE]=0;
}

void processSetButton(){
	if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON) return;
	//Serial.println(F("SET_BUTTON"));
	lastButtonTime=millis();
	if (current_clock_data[D_MODE]<200) current_clock_data[D_MODE]=200;
	else current_clock_data[D_MODE]++;
	hum_needed=0;
	if (current_clock_data[D_MODE]>204) {
		current_clock_data[D_MODE]=0;
		hum_needed=1;
	}
}

void processPlusButton(){
	if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON) return;
	lastButtonTime=millis();
	//Serial.println(F("PLUS_BUTTON"));
	RTC.read(tm);
	boolean update_needed=false;
	switch(current_clock_data[D_MODE]){
		case SET_HOUR:
			if (tm.Hour<23) tm.Hour++;
			else tm.Hour=0;
			update_needed=true;
			break;
		case SET_MIN:
			if (tm.Minute<59) tm.Minute++;
			else tm.Minute=0;
			tm.Second=0;
			update_needed=true;
			break;
		case SET_MONTH:
			if (tm.Month<12) tm.Month++;
			else tm.Month=0;
			update_needed=true;
			break;
		case SET_DAY:
			if (tm.Day<31) tm.Day++;
			else tm.Day=0;
			update_needed=true;
			break;
		case SET_YEAR:
			if (tmYearToCalendar(tm.Year)<2024) tm.Year++;
			else tm.Year=CalendarYrToTm(2016);
			update_needed=true;
			break;
	}
	if (update_needed) {
		setTime(makeTime(tm)); 
		RTC.set(now());
		//Serial.println(F("Time updated"));
	}
}

void processMinusButton(){
	if ((millis() - lastButtonTime) < BOUNCE_TIME_BUTTON) return;
	//Serial.println(F("MINUS_BUTTON"));
	lastButtonTime=millis();
	RTC.read(tm);
	boolean update_needed=false;
	switch(current_clock_data[D_MODE]){
		case SET_HOUR:
			if (tm.Hour>0) tm.Hour--;
			else tm.Hour=23;
			update_needed=true;
			break;
		case SET_MIN:
			if (tm.Minute>0) tm.Minute--;
			else tm.Minute=59;
			tm.Second=0;
			update_needed=true;
			break;
		case SET_MONTH:
			if (tm.Month>0) tm.Month--;
			else tm.Month=12;
			update_needed=true;
			break;
		case SET_DAY:
			if (tm.Day>0) tm.Day--;
			else tm.Day=31;
			update_needed=true;
			break;
		case SET_YEAR:
			if (tmYearToCalendar(tm.Year)>2016) tm.Year--;
			else tm.Year=CalendarYrToTm(2024);
			update_needed=true;
			break;
	}
	if (update_needed) {
		setTime(makeTime(tm));
		RTC.set(now());
		//Serial.println(F("Time updated"));
	}
}

bool getTime(const char *str)
{
  int Hour, Min, Sec;

  if (sscanf(str, "%d:%d:%d", &Hour, &Min, &Sec) != 3) return false;
  tm.Hour = Hour;
  tm.Minute = Min;
  tm.Second = Sec;
  return true;
}

bool getDate(const char *str)
{
  char Month[12];
  int Day, Year;
  uint8_t monthIndex;

  if (sscanf(str, "%s %d %d", Month, &Day, &Year) != 3) return false;
  for (monthIndex = 0; monthIndex < 12; monthIndex++) {
    if (strcmp(Month, monthName[monthIndex]) == 0) break;
  }
  if (monthIndex >= 12) return false;
  tm.Day = Day;
  tm.Month = monthIndex + 1;
  tm.Year = CalendarYrToTm(Year);
  return true;
}
