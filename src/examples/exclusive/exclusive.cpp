#include <iostream>
#include "../../../src/ao/aobject.h"
#include "sysinfoapi.h"

ao_time now_val;
ao_time* get_now() {
	now_val = GetTickCount();
	return &now_val;
}

void log(char v) {
	ao_time time;
	time = *ao_get_now();
	DWORD ticks = time;
	DWORD milliseconds = ticks % 1000;
	ticks /= 1000;
	DWORD seconds = ticks % 60;
	ticks /= 60;
	DWORD minutes = ticks % 60;
	ticks /= 60;
	DWORD hours = ticks % 24;
	printf("%d:%02d:%02d.%03d - %c\n", hours, minutes, seconds, milliseconds, v);	
}

class Res1: public ExclusiveResource {
private:
	ao_time nextTime;	
public:
	
	bool request(Listener& listener) {
		if(!lock(listener)) return false;
		if(start()) {
			// start operation
			delay(nextTime, 1000); // operation delay emulation
			setBusy(true);
		} else {
			if(!isBusy()) {
				// operation is completed
				unlock();
				return true;
			}
		}
		return false;
	}
	
	void process() {
		if(isBusy() && reached(nextTime)) {
			// operation complete emulation
			setBusy(false);
		}
		ExclusiveResource::process();
	}
};

Res1 res1;
GPIO res2(5);

void GPIO::set(int val) {
	if(val) {
		log(pin);
	} else {
		log(-pin);
	}	
}

class Test1: public AObject {
AOListener listener;
ao_time nextTime;
char name;
bool pause;
public:
	Test1(char name) : listener(this) {
		this->name = name;
	}
	void process() {
		if(!pause || reached(nextTime)) {
			pause = false;
			if(res2.lock(listener)) {
				if(res2.start()) {
					log(name);
					delay(nextTime, 1000);
				} else {
					if(clearReached(nextTime)) {
						res2.unlock();						
						delay(nextTime, 20);
						pause = true;
					}					
				}
			}
		}
	}	
};

Test1 test1('1');
Test1 test2('2');
//Test1 test3('3');

int main(int argc, char** argv) {
	if(true) {	
	    ao_get_now = get_now;
		while(true) {
			ao_loop();	
		}
	} else {
		ao_emulate(0, 10000);
	}
};


