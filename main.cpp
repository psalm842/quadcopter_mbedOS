#include "mbed.h"

PwmOut pwmOut1(PTD0);
InterruptIn sw(PTC4);
uint32_t riseCounter = 0;
uint32_t fallCounter = 0;
const uint16_t pwmFreqCalculationPeriod = 10; //ms

void rise_handler(void) {
    riseCounter++; 
}

void fall_handler(void) {
    fallCounter++; 
}

void periodCalculator(void) {
	float frequency = (float)riseCounter/float(pwmFreqCalculationPeriod)*1000;
	//TODO: Calculate pulseWidth
}


int main() {
	RtosTimer periodTimer(periodCalculator, osTimerPeriodic);
    
    periodTimer.start(pwmFreqCalculationPeriod);
    
	pwmOut1.period(0.0005f);
	pwmOut1.write(0.5f);
    sw.rise(rise_handler);
    sw.fall(fall_handler);

    Thread::wait(osWaitForever);
}