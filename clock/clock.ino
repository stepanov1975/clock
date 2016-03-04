#include "myclock.h"
//#include <MemoryFree.h>
// Similar to F(), but for PROGMEM string pointers rather than literals
#define F2(progmem_ptr) (const __FlashStringHelper *)progmem_ptr

myclock clock;

void setup() {
	Serial.begin(9600);
	clock.init();
	while(clock.data_ready==0){
		clock.receive_data();
	}
	clock.data_ready=0;
	clock.print_status();
	delay(4000);
	clock.update();
}

void loop() {
    //Serial.println(freeMemory());
	clock.receive_data();
	if(clock.data_ready==1) {
		clock.data_ready=0;
		//clock.setlocaltime();
	}
	clock.update();
}
