#include "mbed.h"
#include "fsl_ftm.h"
#include "fsl_port.h"
#include "hal/pinmap.h"
#include "MK64F12.h"

// Radio Frequency Receiver Output: Period is 18.4ms (~54Hz), Duty cycle is 8%, varies between 6% to 10%
// Motor Control output signal: Period is 2.5 ms (~400Hz), Duty cycle is 44% @ 0 thrust (up to 50% seems to do nothing), 80% duty cycle is 100% thrust (at least from flight controller)

// NOTE: Only FTM channels 0-3 can be used for input capture. Channels 4-7 can be used for PWM output
// 5 Radio Receiver input Pins: PTD0 (FTM3_Ch0), PTC4 (FTM0_Ch3), PTC3 (FTM0_Ch2), PTC2 (FTM0_Ch1), PTC1 (FTM0_Ch0)
// 4 Motor Control Output Pins: PTA0 (FTM0_Ch5), PTA2(FTM0_Ch7), PTA1(FTM0_Ch6), PTC12 (FTM3_FLT0)

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Get source clock for FTM driver */
#define FTM_SOURCE_CLOCK            CLOCK_GetFreq(kCLOCK_BusClk)

#define FTM_READ_INPUT_CAPTURE  FTM3->CONTROLS[kFTM_Chnl_0].CnV

PwmOut pwmOut1(PTA2);

uint16_t CnV;
uint16_t CnVPrev;
float pwmDutyCycle = 0.5f;

void calculatePeriod(void) {
    pwmDutyCycle += 0.01f;
    pwmDutyCycle = pwmDutyCycle > .8 ? 0.3 : pwmDutyCycle;
    pwmOut1.write(pwmDutyCycle);

    printf("Duty Cycle is %f\r\n",pwmDutyCycle);
	//float frequency = (float)FTM_READ_INPUT_CAPTURE/float(pwmFreqCalculationPeriod)*1000;
	//float period = 1/frequency;
    CnVPrev = CnV;
    CnV = FTM_READ_INPUT_CAPTURE;
    uint32_t status = FTM_GetStatusFlags(FTM3);
    if (status & kFTM_TimeOverflowFlag) {
        FTM_ClearStatusFlags(FTM3, status);
    }

    if (CnV != CnVPrev)
    {
        //printf("Channel Value is: %i\r\n", CnV);
        //printf("Prev Channel Value is: %i\r\n", CnVPrev);
        //printf("Period is: %i\r\n", CnV-CnVPrev);
    }

}


int main() {
    pin_function(PTD0, kPORT_MuxAlt4); // FTM3 Ch0
    pin_mode(PTD0, PullNone);


    uint32_t pwm_base_clock;
    pwm_base_clock = CLOCK_GetFreq(kCLOCK_BusClk);
    float clkval = (float)pwm_base_clock / 1000000.0f;
    uint32_t clkdiv = 0;
    while (clkval > 1) {
        clkdiv++;
        clkval /= 2.0f;
        if (clkdiv == 7) {
            break;
        }
    }

	ftm_config_t ftmInfo;
    FTM_GetDefaultConfig(&ftmInfo);
    ftmInfo.prescale = (ftm_clock_prescale_t)clkdiv;
    // Initialize FTM module
    FTM_Init(FTM3, &ftmInfo);

    // Print a note to terminal
    printf("\r\nFTM input capture example\r\n");
    printf("clkval is %i\r\n", clkval);
    printf("clkdiv is %i\r\n", clkdiv);

    FTM_SetupInputCapture(FTM3, kFTM_Chnl_0, kFTM_RisingEdge, 0);
    FTM3->MOD = 0xFFFF; // Modulo register must not be 0 for counter to work, see documentation
    FTM_StartTimer(FTM3, kFTM_SystemClock);

    pwmOut1.period(0.0025f); // 2.5ms
    pwmOut1.write(0.80f);
    wait(5); // Give the motor time to initialize

    // creates a queue to hold 1 events
    EventQueue queue(1*EVENTS_EVENT_SIZE);
    queue.call_every(300,calculatePeriod); //10ms
    queue.dispatch();
}
