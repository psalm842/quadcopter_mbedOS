#include "mbed.h"
#include "fsl_ftm.h"
#include "fsl_port.h"
#include "hal/pinmap.h"

/******************************************************************************* 
 * Definitions 
 ******************************************************************************/  
/* Get source clock for FTM driver */  
#define FTM_SOURCE_CLOCK            CLOCK_GetFreq(kCLOCK_BusClk)  
  
#define FTM_READ_INPUT_CAPTURE  FTM0->CONTROLS[kFTM_Chnl_7].CnV  

PwmOut pwmOut1(PTD0);
uint32_t pwmFreqCalculationPeriod = 100; // ms

void calculatePeriod(void) {
	//float frequency = (float)FTM_READ_INPUT_CAPTURE/float(pwmFreqCalculationPeriod)*1000;
	//float period = 1/frequency;
	printf("Channel Value is: %x\r\n", FTM_READ_INPUT_CAPTURE);
}


int main() {
	pin_function(PTA2, kPORT_MuxAlt4);
	//PORTA->PCR[2] = PORTA->PCR[2] & ~PORT_PCR_MUX_MASK | PORT_PCR_MUX(kPORT_MuxAlt4);
	ftm_config_t ftmInfo;    
  
    /* Print a note to terminal */  
    printf("\r\nFTM input capture example\r\n");  
    
    //printf("Port A, PCR2 Register Value: %x",PORTA->PCR[2]);
  
    FTM_GetDefaultConfig(&ftmInfo);  
    /* Initialize FTM module */  
    FTM_Init(FTM0, &ftmInfo);
  
    FTM_SetupInputCapture(FTM0, kFTM_Chnl_7, kFTM_RisingEdge, 0);
    FTM0->MOD = 0xFFFF;
    FTM_StartTimer(FTM0, kFTM_SystemClock);
    
	// creates a queue to hold 1 events
    EventQueue queue(1*EVENTS_EVENT_SIZE);
    queue.call_every(pwmFreqCalculationPeriod,calculatePeriod);
    queue.dispatch();

	pwmOut1.period(100);
	pwmOut1.write(0.5f);
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
