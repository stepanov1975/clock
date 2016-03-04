#include "myclock.h"
void myclock::init()
{
	Serial.println(F("Initialisation started"));
    matrix.begin();
    matrix.cleanScreen();
	
	pinMode(INPUT_CLOCK, INPUT);
	pinMode(DATA1, INPUT);
	pinMode(DATA2, INPUT);
	pinMode(OUTPUT_CLOCK, OUTPUT);
	digitalWrite(OUTPUT_CLOCK,LOW);
	Serial.println(F("Initialisation finished"));
	int testdelay=100;
	for (byte row=0; row<16; row++){
		matrix.cleanScreen();
		for (byte col=0; col<32; col++){
			matrix.drawPixel(col, row, 1,0,0);
		}
		matrix.swapBuffers(true);
		delay(testdelay);
	}
	for (byte row=0; row<16; row++){
		matrix.cleanScreen();
		for (byte col=0; col<32; col++){
			matrix.drawPixel(col, row, 0,1,0);
		}
		matrix.swapBuffers(true);
		delay(testdelay);
	}
	for (byte row=0; row<16; row++){
		matrix.cleanScreen();
		for (byte col=0; col<32; col++){
			matrix.drawPixel(col, row, 0,0,1);
		}
		matrix.swapBuffers(true);
		delay(testdelay);
	}
	//Serial.begin(9600);
	
	current_clock_data[brightness_index]=1;
	current_clock_data[screen_mode_index]=2;
	current_clock_data[error_index]==99;
}

void myclock::print_status(){
	matrix.cleanScreen();
	if (current_clock_data[error_index]==0){
		put_tiny_string_from_progmem(0, 0, StatusMessage,0, 1,0);
		put_tiny_string_from_progmem(0, 7, OkMessage,0, 1,0);
	}
	else{
		put_tiny_string_from_progmem(0, 0, StatusMessage,0, 1,0);
		put_tiny_string_from_progmem(0, 7, ErrorMessage,0, 1,0);
		putCharTiny(20, 8, '0'+current_clock_data[error_index]/10,  0, 1,0);
		putCharTiny(24, 8, '0'+current_clock_data[error_index]%10,  0, 1,0);
	}
	matrix.swapBuffers(true);
}

void myclock::update()
{
	//putLargeTime(2,2,current_clock_data[hour_index],current_clock_data[min_index],current_clock_data[sec_index],0,0,1);
	//putSmallTime(-1,0,current_clock_data[hour_index],current_clock_data[min_index],current_clock_data[sec_index],0,0,1);
	//tinyTime(1,0,current_clock_data[hour_index],current_clock_data[min_index],current_clock_data[sec_index],0,0,1);
	//put8x8Time(0,0,current_clock_data[hour_index],current_clock_data[min_index],current_clock_data[sec_index],0,0,1);
	g=1;//current_clock_data[brightness_index];
	//Serial.println(current_clock_data[screen_mode_index]);
	switch(current_clock_data[screen_mode_index]){
		case 0:
			matrix.cleanScreen();
			clock_type1();
			break;
		case 1:
			matrix.cleanScreen();
			clock_type2();
			break;
		case 2:
			matrix.cleanScreen();
			clock_type3();
			break;
		case 3:
			print_status();
			break;
		case SET_HOUR:
		case SET_MIN:
			matrix.cleanScreen();
			setlocaltime();
			set_time();
			break;
		case SET_MONTH:
		case SET_DAY:
			matrix.cleanScreen();
			setlocaltime();
			set_date();
			break;
		case SET_YEAR:
			matrix.cleanScreen();
			setlocaltime();
			set_year();
			break;
		default:
			clock_type1();
	}
	
}

void myclock::set_time(){
	//Serial.println(F("set_time"));
	putRusChar(1, 1, 22, r,  g, b);
	putRusChar(7, 1, 50, r,  g, b);
	putRusChar(13, 1, 37, r,  g, b);
	putRusChar(19, 1, 45, r,  g, b);
	putRusChar(25, 1, 62, r,  g, b);
	putNormalTime(1,second_row_y,hour(),minute(),second(),r,g,b);
	matrix.swapBuffers(true);
}

void myclock::set_date(){
	//Serial.println(F("set_date"));
	putRusChar(1, 1, 4, r,  g, b);
	putRusChar(7, 1, 33, r,  g, b);
	putRusChar(13, 1, 52, r,  g, b);
	putRusChar(19, 1, 33, r,  g, b);
	putDate(date_offset,second_row_y,day(),month(),r,g,b);
	matrix.swapBuffers(true);
}

void myclock::set_year(){
	//Serial.println(F("set_year"));
	putRusChar(1, 1, 7, r,  g, b);
	putRusChar(7, 1, 47, r,  g, b);
	putRusChar(13, 1, 36, r,  g, b);

	putYear(year_offset,second_row_y,year()-2000,r,g,b);
	matrix.swapBuffers(true);
}

void myclock::clock_type2(){
	//Serial.println(F("clock_type2"));
	matrix.cleanScreen();
	putLargeTime(2,2,hour(),minute(),second(),r,g,b);
	//tinyTime(1,0,hour(),minute(),second(),r,g,b);
	matrix.swapBuffers(true);
}

void myclock::clock_type3(){
	//Serial.println(F("clock_type2"));
	matrix.cleanScreen();
	tinyTime(1,-1,hour(),minute(),second(),r,g,b);
	putTinyDate(1,5,day(),month(),year()-2000,r,g,b);
	matrix.swapBuffers(true);
}

void myclock::clock_type1(){
	//Serial.println(F("clock_type1"));
	short int shiftdelay=20;
	short int current_delay=0;
	//short int year_offset=2,
	//	temperature_offset=4,
	//	date_offset=0,
	//	humidity_offset=6,
	//	second_row_y=8;
	
	if (first_shift_done==0){
		first_shift_done=1;
		shift_in_progress=1;
		last_shift_sec=second();
		shift_offset=31;
	}
	
	if (second()>=last_shift_sec){
		current_delay=second()-last_shift_sec;
	}
	else {
		current_delay=(60-last_shift_sec)+second();
	}
	
	if (current_delay>shiftdelay) {
		shift_in_progress=1;
		clock1_state++;
		if (clock1_state>3) clock1_state=0;
	}
	
	if (shift_in_progress==1){
		for (short int shift=shift_offset;shift>0;shift--){
			matrix.cleanScreen();
			switch(clock1_state){
				case 0:
					putYear(year_offset-(shift_offset-shift),second_row_y,year()-2000,r,g,b);
					putTemp(temperature_offset+shift,second_row_y,current_clock_data[temp_index],r,g,b);
					break;
				case 1:
					putTemp(temperature_offset-(shift_offset-shift),second_row_y,current_clock_data[temp_index],r,g,b);
					putHumidity(humidity_offset+shift,second_row_y,current_clock_data[humidity_index],r,g,b);
					break;
				case 2:
					putHumidity(humidity_offset-(shift_offset-shift),second_row_y,current_clock_data[humidity_index],r,g,b);
					putDate(date_offset+shift,second_row_y,day(),month(),r,g,b);
					break;
				case 3:
					putDate(date_offset-(shift_offset-shift),second_row_y,day(),month(),r,g,b);
					putYear(year_offset+shift,second_row_y,year()-2000,r,g,b);
					break;
			}
			putNormalTime(1,-1,hour(),minute(),second(),r,g,b);
			matrix.swapBuffers(true);
			//while(data_ready!=1) receive_data();
			data_ready=0;
			//delay(5);
		}
		shift_in_progress=0;
		last_shift_sec=second();
	}
	else {
		matrix.cleanScreen();
		switch(clock1_state){
			case 0:
				putTemp(temperature_offset,second_row_y,current_clock_data[temp_index],r,g,b);
				break;
			case 1:
				putHumidity(humidity_offset,second_row_y,current_clock_data[humidity_index],r,g,b);
				break;
			case 2:
				putDate(date_offset,second_row_y,day(),month(),r,g,b);
				break;
			case 3:
				putYear(year_offset,second_row_y,year()-2000,r,g,b);
				break;
		}
	}
	putNormalTime(1,-1,hour(),minute(),second(),r,g,b);
	matrix.swapBuffers(true);
}

void  myclock::putTempHum(int x, int y, uint8_t& t,uint8_t& h, uint8_t r, uint8_t g,uint8_t b){
	//Width =6
	putNormalChar(x, y, '0'+t/10, r,g,b);
	putNormalChar(x+6, y, '0'+t%10, r,g,b);
	putNormalChar(x+12, y, 127, r,g,b);
	
	putNormalChar(x+15, y, '0'+h/10, r,g,b);
	putNormalChar(x+21, y, '0'+h%10, r,g,b);
	putNormalChar(x+27, y, '%', r,g,b);
}

void  myclock::putTemp(int x, int y, uint8_t& t, uint8_t r, uint8_t g,uint8_t b){
	//Width =6
	putNormalDigit(x, y, '0'+t/10, r,g,b);
	putNormalDigit(x+7, y, '0'+t%10, r,g,b);

	putNormalChar(x+14, y, 127, r,g,b);
	putNormalDigit(x+18, y, 58, r,g,b);
	
}

void  myclock::putHumidity(int x, int y, uint8_t& h, uint8_t r, uint8_t g,uint8_t b){
	//Width =6
	putNormalDigit(x, y, '0'+h/10, r,g,b);
	putNormalDigit(x+7, y, '0'+h%10, r,g,b);
	putNormalDigit(x+14, y, 59, r,g,b);
	
}
void  myclock::putDate(int x, int y, int d,int m, uint8_t r, uint8_t g,uint8_t b){
	//Width =6
	if (!is_set_day() | myblink()){
		putNormalDigit(x, y, '0'+d/10, r,g,b);
		putNormalDigit(x+7, y, '0'+d%10, r,g,b);
	}
	else{
		putNormalDigit(x, y, '0'+d/10, 0,0,0);
		putNormalDigit(x+7, y, '0'+d%10, 0,0,0);
	}
	//putNormalChar(x+14, y, '/', r,g,b);
	matrix.drawPixel(x+14, y+7, r,g,b);
	matrix.drawPixel(x+14, y+6, r,g,b);
	matrix.drawPixel(x+15, y+7, r,g,b);
	matrix.drawPixel(x+15, y+6, r,g,b);
	if(!is_set_month() | myblink()){
		putNormalDigit(x+17, y, '0'+m/10, r,g,b);
		putNormalDigit(x+24, y, '0'+m%10, r,g,b);
	}
	else{
		putNormalDigit(x+17, y, '0'+m/10, 0,0,0);
		putNormalDigit(x+24, y, '0'+m%10, 0,0,0);
	}
}
void  myclock::putYear(int x, int y, int a, uint8_t r, uint8_t g,uint8_t b){
	//Width =6
	if(!is_set_year() | myblink()){
		putNormalDigit(x, y, '2', r,g,b);
		putNormalDigit(x+7, y, '0', r,g,b);
		putNormalDigit(x+14, y, '0'+a/10, r,g,b);
		putNormalDigit(x+21, y, '0'+a%10, r,g,b);
	}
	else{
		putNormalDigit(x, y, '2', 0,0,0);
		putNormalDigit(x+7, y, '0', 0,0,0);
		putNormalDigit(x+14, y, '0'+a/10, 0,0,0);
		putNormalDigit(x+21, y, '0'+a%10, 0,0,0);
	}
	
}

//----------------------------------------------------------
//Time display functions

void myclock::tinyTime(uint8_t x, uint8_t y, int h,int m,int s, uint8_t r, uint8_t g,uint8_t b){
	//Width =4
	putCharTiny(x, y, '0'+h/10, r,g,b);
	putCharTiny(x+4, y, '0'+h%10, r,g,b);
	
	matrix.drawPixel(x+9, y+2, r,g,b);
	matrix.drawPixel(x+9, y+4, r,g,b);

	putCharTiny(x+10, y, '0'+m/10, r,g,b);
	putCharTiny(x+14, y, '0'+m%10, r,g,b);

	matrix.drawPixel(x+19, y+2, r,g,b);
	matrix.drawPixel(x+19, y+4, r,g,b);

	putCharTiny(x+20, y, '0'+s/10, r,g,b);
	putCharTiny(x+24, y, '0'+s%10, r,g,b);
}

void myclock::putTinyDate(int x, int y, int d,int m,int yr, uint8_t r, uint8_t g,uint8_t b){
	putCharTiny(x, y, '0'+d/10, r,g,b);
	putCharTiny(x+4, y, '0'+d%10, r,g,b);
	
	matrix.drawPixel(x+9, y+5, r,g,b);

	putCharTiny(x+10, y, '0'+m/10, r,g,b);
	putCharTiny(x+14, y, '0'+m%10, r,g,b);

	matrix.drawPixel(x+19, y+5, r,g,b);

	putCharTiny(x+20, y, '0'+yr/10, r,g,b);
	putCharTiny(x+24, y, '0'+yr%10, r,g,b);
}

void myclock::putLargeTime(uint8_t x, uint8_t y, int h,int m,int s, uint8_t r, uint8_t g,uint8_t b)
{
	//Width =6 
	putDigitLarge(x, y, '0'+h/10, r,g,b);
	putDigitLarge(x+6, y, '0'+h%10, r,g,b);

	putDigitLarge(x+15, y, '0'+m/10, r,g,b);
	putDigitLarge(x+21, y, '0'+m%10, r,g,b);

	if (s%2==1) {
		r=0;
		g=0;
		b=0;
	}
	
	matrix.drawPixel(x+13, y+2, r,g,b);
	matrix.drawPixel(x+14, y+2, r,g,b);
	matrix.drawPixel(x+13, y+3, r,g,b);
	matrix.drawPixel(x+14, y+3, r,g,b);

	matrix.drawPixel(x+13, y+8, r,g,b);
	matrix.drawPixel(x+14, y+8, r,g,b);
	matrix.drawPixel(x+13, y+9, r,g,b);
	matrix.drawPixel(x+14, y+9, r,g,b);
}

void myclock::putSmallTime(int x, int y, int h,int m,int s, uint8_t r, uint8_t g,uint8_t b)
{
	//Width =5
	putSmallChar(x, y, '0'+h/10, r,g,b);
	putSmallChar(x+5, y, '0'+h%10, r,g,b);
	
	matrix.drawPixel(x+11, y+2, r,g,b);
	matrix.drawPixel(x+11, y+4, r,g,b);

	putSmallChar(x+12, y, '0'+m/10, r,g,b);
	putSmallChar(x+17, y, '0'+m%10, r,g,b);

}

void  myclock::putNormalTime(short int x,short int y, int h,int m,int s, uint8_t r, uint8_t g,uint8_t b){
	//Width =6
	
	if (!is_set_hour() | myblink()){
		putNormalDigit(x, y, '0'+h/10, r,g,b);
		putNormalDigit(x+7, y, '0'+h%10, r,g,b);
	}
	else{
		putNormalDigit(x, y, '0'+h/10, 0,0,0);
		putNormalDigit(x+7, y, '0'+h%10,  0,0,0);
	}

	if (!is_set_min() | myblink()){
		putNormalDigit(x+16, y, '0'+m/10, r,g,b);
		putNormalDigit(x+23, y, '0'+m%10, r,g,b);
	}
	else{
		putNormalDigit(x+16, y, '0'+m/10, 0,0,0);
		putNormalDigit(x+23, y, '0'+m%10, 0,0,0);
	}
	
	if (myblink() & !is_set_hour() & !is_set_min()) {
		r=0;
		g=0;
		b=0;
	}
	
	matrix.drawPixel(x+14, y+2, r,g,b);
	matrix.drawPixel(x+14, y+6, r,g,b);
	//matrix.drawPixel(0, 0, r,g,b);
}

void myclock::put_tiny_string_from_progmem(uint8_t x, uint8_t y, const char message[],uint8_t r, uint8_t g,uint8_t b){
	char c;
	if(!message) return;
	int i=0;
	while((c = pgm_read_byte(message++))){
		putCharTiny(x+4*i, y, c,  r,  g, b);
		i++;
	}
}
//----------------------------------------------------------
//Simple char functions
void myclock::putNormalDigit(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b){
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex	=	c - 48;
	putChar( x,  y,  small_digits[charIndex], 8, 6,  r,  g, b);
}

void myclock::putNormalChar(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b){
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex	=	c - 32;
	putChar( x,  y,  myfont[charIndex], 8, 6,  r,  g, b);
}

void  myclock::putSmallChar(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b)
{
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex	=	c - 32;
	putChar( x,  y,  smallFont[charIndex], 8, 6,  r,  g, b);
}

void myclock::putDigitLarge(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b)
{
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex	=	c - '0'+22;
	putChar( x,  y,  bigFont[charIndex], 12, 6,  r,  g, b);
}

void myclock::putCharTiny(uint8_t x, uint8_t y, char c, uint8_t r, uint8_t g,uint8_t b)
{
	// fonts defined for ascii 32 and beyond (index 0 in font array is ascii 32);
	byte charIndex	=	c - 32;
	putChar( x,  y,  tinyFont[charIndex], 8, 4,  r,  g, b);
}

void myclock::putDigit(uint8_t x, uint8_t y, const unsigned char * c,uint8_t h,uint8_t w, uint8_t r, uint8_t g,uint8_t b){
	for (byte row=0; row<h; row++)
	{
		byte rowDots	=	pgm_read_byte_near(&c[row]);
		for (byte col=0; col<w; col++)
		{
			if (rowDots & (1<<(w-col-1)))
				matrix.drawPixel(x+col, y+row, r,g,b);
			else
				matrix.drawPixel(x+col, y+row, 0,0,0);
		}
	}
}

void myclock::putChar(uint8_t x, uint8_t y, const unsigned char * c,uint8_t h,uint8_t w, uint8_t r, uint8_t g,uint8_t b){
	for (byte row=0; row<h; row++)
	{
		byte rowDots	=	pgm_read_byte_near(&c[row]);
		for (byte col=0; col<w; col++)
		{
			if (rowDots & (1<<(w-col-1)))
				matrix.drawPixel(x+col, y+row, r,g,b);
			//else
			//	matrix.drawPixel(x+col, y+row, 0,0,0);
		}
	}
}

void myclock::putRusChar(uint8_t x, uint8_t y, int charIndex, uint8_t r, uint8_t g,uint8_t b){
	for (byte col=0; col<5; col++)
	{
		byte rowDots	=	pgm_read_byte_near(ASCII_RUS[charIndex]+col);
		for (byte row=0; row<8; row++)
		{
			if (rowDots & (1<<(row)))
				matrix.drawPixel(x+col, y+row, r,g,b);
			else
				matrix.drawPixel(x+col, y+row, 0,0,0);
		}
	}
}

myclock::myclock() : matrix(A, B, C, CLK, LAT, OE, true)
{
    // Last parameter = 'true' enables double-buffering, for flicker-free,
    // buttery smooth animation.  Note that NOTHING WILL SHOW ON THE DISPLAY
    // until the first call to swapBuffers().  This is normal.
    // Double-buffered mode consumes nearly all the RAM available on the
    // Arduino Uno -- only a handful of free bytes remain.
};

void myclock::receive_data() {
	uint8_t input_clock_state=digitalRead(INPUT_CLOCK);
	if (out_clk_state==input_clock_state){
		
		uint8_t data1_val=digitalRead(DATA1);
		uint8_t data2_val=digitalRead(DATA2);
		
		//Serial.print("I received: ");
		//Serial.print(data1_val, DEC);
		//Serial.println(data2_val, DEC);
		
		if (out_clk_state==HIGH) {
			digitalWrite(OUTPUT_CLOCK,LOW);
			out_clk_state=LOW;
		}
		else {
			digitalWrite(OUTPUT_CLOCK,HIGH);
			out_clk_state=HIGH;
		}
		
		if (sync_ok==0){//Looking for sync byte
			current_byte=(current_byte<<1)|data1_val;
			if (current_byte==HEADER_BYTE){
				sync_ok=1;
				current_byte=data2_val;
				bit_counter=1;
				//Serial.println("Sync OK");
			}
			else {
				current_byte=(current_byte<<1)|data2_val;
				if (current_byte==HEADER_BYTE){
					sync_ok=1;
					current_byte=0;
					//Serial.println("Sync");
				}
			}
		}
		else {//receiving data
			push_bit(data1_val);
			if ((sync_ok==1)) push_bit(data2_val);
			else {
				current_byte=data2_val;
			}
		}
	}
}

void myclock::setlocaltime(){
	short unsigned int thr=current_clock_data[hour_index];
	short unsigned int tmin=current_clock_data[min_index];
	short unsigned int tsec=current_clock_data[sec_index];
	short unsigned int tday=current_clock_data[day_index];
	short unsigned int tmonth=current_clock_data[month_index];
	unsigned int tyr=current_clock_data[year_index]+2000;
	setTime(thr,tmin,tsec,tday,tmonth,tyr);
	Serial.println(F("TU"));
}

void myclock::update_local_time(){
	if (system_time_set!=1){
		setlocaltime();
		system_time_set=1;
	}
	else {
		if (current_clock_data[sec_index]==0 & current_clock_data[min_index]==0){
			setlocaltime();
		}
	}
}

inline void myclock::push_bit(uint8_t in_bit){
	current_byte|=in_bit;
	bit_counter++;
	
	if (bit_counter>7){
		current_clock_data[byte_counter]=current_byte;
		//Serial.println(current_byte,DEC);
		bit_counter=0;
		current_byte=0;
		byte_counter++;
		if(byte_counter>=NUMOFDADABYTES) {
			byte_counter=0;
			sync_ok=0;
			data_ready=1;
			update_local_time();
		}
	}
	else {
		current_byte=current_byte<<1;
	}
}