#include "mbed.h"

PwmOut pwmOut1(PTD0);
InterruptIn sw(PTC4);
uint32_t riseCounter = 0;
uint32_t fallCounter = 0;

void rise_handler(void) {
    riseCounter++; 
}

void fall_handler(void) {
    fallCounter++; 
}

int main() {
	pwmOut1.period(0.0005f);
	pwmOut1.write(0.5f);
    sw.rise(rise_handler);
    sw.fall(fall_handler);
}