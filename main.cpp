#include "mbed.h"
#include "fsl_ftm.h"
#include "fsl_port.h"
#include "hal/pinmap.h"
#include "MK64F12.h"

/******************************************************************************* 
 * Definitions 
 ******************************************************************************/  
/* Get source clock for FTM driver */  
#define FTM_SOURCE_CLOCK            CLOCK_GetFreq(kCLOCK_BusClk)  
  
#define FTM_READ_INPUT_CAPTURE  FTM3->CONTROLS[kFTM_Chnl_0].CnV  

PwmOut pwmOut1(PTA2);

void calculatePeriod(void) {
	//float frequency = (float)FTM_READ_INPUT_CAPTURE/float(pwmFreqCalculationPeriod)*1000;
	//float period = 1/frequency;
	printf("Channel Value is: %i\r\n", FTM_READ_INPUT_CAPTURE);
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

    FTM_SetupInputCapture(FTM3, kFTM_Chnl_0, kFTM_RisingEdge, 0);
    FTM3->MOD = 0xFFFF; // Modulo register must not be 0 for counter to work, see documentation
    FTM_StartTimer(FTM3, kFTM_SystemClock);

    pwmOut1.period(0.010f); // 10ms
    pwmOut1.write(0.50f);
    
    // creates a queue to hold 1 events
    EventQueue queue(1*EVENTS_EVENT_SIZE);
    queue.call_every(20,calculatePeriod); //20ms 
    queue.dispatch();
}
