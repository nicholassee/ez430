#include <msp430.h> 
#include "display.h"
#include "timer.h"
#include <stdio.h>
#include "temperature.h"
#include "commonVar.h"
#include "as.h"
#include "acceleration.h"
#include "button.h"
#include "buzzer.h"
/*
 * main.c
 */



// Number of calibration data bytes in INFOA memory
#define CALIBRATION_DATA_LENGTH         (13u)

typedef union
{
    struct
    {
        unsigned char temperature_measurement : 1;  // 1 = Measure temperature
        unsigned short acceleration_measurement : 1; // 1 = Measure acceleration
    } flag;
    unsigned short all_flags;                        // Shortcut to all request flags (for reset)
} s_request_flags;

volatile s_request_flags request;
unsigned short adc12_result;
unsigned char adc12_data_ready;
// to indicate toggle flag
// to indicate toggle flag
//extern volatile int toggle_flag;

// to indicate current mode of the watch
// 0 indicate watch is in warmup mode
// 1 indicate watch is in running mode
volatile char watch_mode;

// to toggle between warmup status
// 0 function status = ready
// 1 function status = active
// 2 function status = done
volatile char warmup_status;

// to toggle running mode
// 0 function status = start/resume
// 1 function status = pause
// 2 function status = reset
volatile char running_status;

// to indicate temperature is updated
volatile char temp_updated_flag;

// to indicate distance is updated
volatile char dist_updated_flag;

volatile int seconds;
volatile int minutes;
volatile int hours;


void init_flag(void){
	// to indicate toggle flag
	//toggle_flag = 0;

	watch_mode = 0;
	warmup_status = 0;
	running_status = 0;

	// to indicate user at warmup application
//	warmup_menu_flag=0;
//
//	// to indicate the process/state of warmp up
//	warmup_process_flag=0;
//
//	// to indicate user at running application
//	running_menu_flag=1;
//
//	// to indicate start/stop
//	start_stop_running_flag=0;

	// to indicate temperature is updated
	temp_updated_flag=0;

	// to indicate distance is updated
	dist_updated_flag=0;

}




// *************************************************************************************************
// @fn          process_requests
// @brief       Process requested actions outside ISR context.
// @param       none
// @return      none
// *************************************************************************************************
void process_requests(void)
{
    // Do temperature measurement
//    if (request.flag.temperature_measurement)
//        temperature_measurement(FILTER_ON);

    // Do acceleration measurement
//    if (request.flag.acceleration_measurement)
//        do_acceleration_measurement();

    // Reset request flag
    request.all_flags = 0;
}

// *************************************************************************************************
// @fn          display_update
// @brief       Process display flags and call LCD update routines.
// @param       none
// @return      none
// *************************************************************************************************
void display_update(void)
{
	//display_temperature(LCD_SEG_L1_3_1,DISPLAY_LINE_UPDATE_FULL);
    // Clear display flag
    //display.all_flags = 0;
}

// *************************************************************************************************
// @fn          to_lpm
// @brief       Go to LPM0/3.
// @param       none
// @return      none
// *************************************************************************************************
void to_lpm(void)
{
    // Go to LPM3
    _BIS_SR(LPM3_bits + GIE);
    __no_operation();
}

// *************************************************************************************************
// @fn          idle_loop
// @brief       Go to LPM. Service watchdog timer when waking up.
// @param       none
// @return      none
// *************************************************************************************************
void idle_loop(void)
{
    // To low power mode
    to_lpm();

#ifdef USE_WATCHDOG
    // Service watchdog
    WDTCTL = WDTPW + WDTIS__512K + WDTSSEL__ACLK + WDTCNTCL;
#endif
}

// *************************************************************************************************
// @fn          read_calibration_values
// @brief       Read calibration values for temperature measurement, voltage measurement
//                              and radio from INFO memory.
// @param       none
// @return      none
// *************************************************************************************************
void read_calibration_values(void)
{
    unsigned char cal_data[CALIBRATION_DATA_LENGTH]; // Temporary storage for constants
    unsigned char i;
    unsigned char *flash_mem;                        // Memory pointer
    // Read calibration data from Info D memory
    flash_mem = (unsigned char *) 0x1800;
    for (i = 0; i < CALIBRATION_DATA_LENGTH; i++)
    {
        cal_data[i] = *flash_mem++;
    }

    if (cal_data[0] == 0xFF)
    {
        // If no values are available (i.e. INFO D memory has been erased by user), assign
        // experimentally derived values
        sTemp.offset = -250;

    } else
    {
        sTemp.offset = (short) ((cal_data[2] << 8) + cal_data[3]);

    }
}


//MAIN PROGRAM
int main(void) {
    // Watchdog triggers after 16 seconds when not cleared
#ifdef USE_WATCHDOG
    WDTCTL = WDTPW + WDTIS__512K + WDTSSEL__ACLK;
#else
    WDTCTL = WDTPW + WDTHOLD;
#endif

    // Runner Companion Watch Initialization
	lcd_init();
	init_buzzer();
	Timer0_Init();
	Timer1_Init();
	read_calibration_values();
	init_flag();
	buttons_init();
	// Init acceleration sensor
	as_init();
	// Reset acceleration measurement
	reset_acceleration();
	init_pedo();

	to_lpm();

}

