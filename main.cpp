#include "mbed.h"

PwmOut pwmOut1(PTD0);
InterruptIn sw(PTC4);
uint32_t riseCounter = 0;
uint32_t fallCounter = 0;
const uint16_t pwmFreqCalculationPeriod = 10; //ms

void countRisingEdges(void) {
    riseCounter++; 
}

void countFallingEdges(void) {
    fallCounter++; 
}

void periodCalculator(void) {
	float frequency = (float)riseCounter/float(pwmFreqCalculationPeriod)*1000;
	//TODO: Calculate pulseWidth
}


int main() {
	printf("Testing 1,2,3...");

	// creates a queue to hold 2 events
    EventQueue queue(2*EVENTS_EVENT_SIZE);
    queue.call_every(10,pwmFreqCalculationPeriod);
    
	pwmOut1.period(0.0005f);
	pwmOut1.write(0.5f);
    sw.rise(countRisingEdges);
    sw.fall(countFallingEdges);

    Thread::wait(osWaitForever);
}