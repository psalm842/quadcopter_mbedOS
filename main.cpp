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

//PwmOut pwmOut1(PTD0);
uint32_t pwmFreqCalculationPeriod = 2000; // ms

void calculatePeriod(void) {
	//float frequency = (float)FTM_READ_INPUT_CAPTURE/float(pwmFreqCalculationPeriod)*1000;
	//float period = 1/frequency;
	printf("Channel Value is: %i\r\n", FTM_READ_INPUT_CAPTURE);
}


int main() {
	pin_function(PTA2, kPORT_MuxAlt4);
    pin_mode(PTA2,PullNone);
    pin_function(PTD0, kPORT_MuxAlt4);
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
    /* Initialize FTM module */  
    FTM_Init(FTM0, &ftmInfo);
    FTM_Init(FTM3, &ftmInfo);
    
    /* Print a note to terminal */  
    printf("\r\nFTM input capture example\r\n");  
    //printf("Port A, PCR2 Register Value: %x",PORTA->PCR[2]);

    ftm_chnl_pwm_signal_param_t config = {
        .chnlNumber = kFTM_Chnl_7,
        .level = kFTM_HighTrue,
        .dutyCyclePercent = 20,
        .firstEdgeDelayPercent = 0
    };


    FTM_SetupPwm(FTM0, &config, 1, kFTM_EdgeAlignedPwm, 50, 1);
    FTM_StartTimer(FTM0, kFTM_SystemClock);


    FTM_SetupInputCapture(FTM3, kFTM_Chnl_0, kFTM_RisingEdge, 0);
    //FTM0->MOD = 0xFFFF;
    FTM_StartTimer(FTM3, kFTM_SystemClock);

    // creates a queue to hold 1 events
    EventQueue queue(1*EVENTS_EVENT_SIZE);
    queue.call_every(pwmFreqCalculationPeriod,calculatePeriod);
    queue.dispatch();
}


/*!
 * @brief Enables capturing an input signal on the channel using the function parameters.
 *
 * When the edge specified in the captureMode argument occurs on the channel, the FTM counter is
 * captured into the CnV register. The user has to read the CnV register separately to get this
 * value. The filter function is disabled if the filterVal argument passed in is 0. The filter
 * function is available only for channels 0, 1, 2, 3.
 *
 * @param base        FTM peripheral base address
 * @param chnlNumber  The channel number
 * @param captureMode Specifies which edge to capture
 * @param filterValue Filter value, specify 0 to disable filter. Available only for channels 0-3.
 
void FTM_SetupInputCapture(FTM_Type *base,
                           ftm_chnl_t chnlNumber,
                           ftm_input_capture_edge_t captureMode,
                           uint32_t filterValue);

*/
