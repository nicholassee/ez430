// *************************************************************************************************
//
//      Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
//
//
//        Redistribution and use in source and binary forms, with or without
//        modification, are permitted provided that the following conditions
//        are met:
//
//          Redistributions of source code must retain the above copyright
//          notice, this list of conditions and the following disclaimer.
//
//          Redistributions in binary form must reproduce the above copyright
//          notice, this list of conditions and the following disclaimer in the
//          documentation and/or other materials provided with the
//          distribution.
//
//          Neither the name of Texas Instruments Incorporated nor the names of
//          its contributors may be used to endorse or promote products derived
//          from this software without specific prior written permission.
//
//        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//        LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//        DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//        THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// Temperature measurement functions.
// *************************************************************************************************
#include <msp430.h>

/* A macro that generates a bit mask according to a given bit number.
 * Example:
 * - BIT(0) expands to 1 (== 0x01)
 * - BIT(3) expands to 8 (== 0x08)
 */

#define BIT(x) (1uL << (x))

// LCD Segments
#define LCD_A    BIT4
#define LCD_B    BIT5
#define LCD_C    BIT6
#define LCD_D    BIT7
#define LCD_F    BIT0
#define LCD_G    BIT1
#define LCD_E    BIT2
#define LCD_H    BIT3

// LCD Segment Mapping
const unsigned char  LCD_Char_Map[] =
{
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F,        // '0' or 'O'
  LCD_B+LCD_C,                                // '1' or 'I'
  LCD_A+LCD_B+LCD_D+LCD_E+LCD_G,              // '2' or 'Z'
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_G,              // '3'
  LCD_B+LCD_C+LCD_F+LCD_G,                    // '4' or 'y'
  LCD_A+LCD_C+LCD_D+LCD_F+LCD_G,              // '5' or 'S'
  LCD_A+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,        // '6' or 'b'
  LCD_A+LCD_B+LCD_C,                          // '7'
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_E+LCD_F+LCD_G,  // '8' or 'B'
  LCD_A+LCD_B+LCD_C+LCD_D+LCD_F+LCD_G,        // '9' or 'g'
  LCD_B+LCD_C+LCD_E+LCD_F+LCD_G,              // Displays "X"
  LCD_B+LCD_C+LCD_D+LCD_F+LCD_G,              // Displays "Y"
};
// *************************************************************************************************
// Include section

// system
//#include "project.h"

// driver
#include "display.h"
#include "bmp_as.h"
//#include "cma_as.h"
#include "as.h"

// logic
#include "acceleration.h"
//#include "simpliciti.h"
//#include "user.h"

// *************************************************************************************************
// Global Variable section
struct accel sAccel;

unsigned char			last_readings[7];
unsigned long			steps;
unsigned char			calibrationCounter;
unsigned long			sumOfRead;
unsigned long			calibratedValue;

// Conversion values from data to mgrav taken from BMA250 datasheet (rev 1.05, figure 4)
	const unsigned short bmp_mgrav_per_bit[7] = { 16, 31, 63, 125, 250, 500, 1000 };
// Conversion values from data to mgrav taken from CMA3000-D0x datasheet (rev 0.4, table 4)
	const unsigned short cma_mgrav_per_bit[7] = { 18, 36, 71, 143, 286, 571, 1142 };

// *************************************************************************************************
// Extern section


// *************************************************************************************************
// @fn          reset_acceleration
// @brief       Reset acceleration variables.
// @param       none
// @return      none
// *************************************************************************************************
void reset_acceleration(void)
{
    // Start with Y-axis display
    sAccel.view_style = DISPLAY_ACCEL_Y;

    // Clear timeout counter
    sAccel.timeout = 0;

    // Default mode is off
    sAccel.mode = ACCEL_MODE_OFF;
    steps = 0;
	last_readings[4] = 0;
	last_readings[3] = 0;
	last_readings[2] = 0;
	last_readings[1] = 0;
	last_readings[0] = 0;
}

// *************************************************************************************************
// @fn          sx_acceleration
// @brief       Acceleration direct function. Button UP switches between X/Y/Z values.
// @param       unsigned char line         LINE2
// @return      none
// *************************************************************************************************
void sx_acceleration()
{
    if (++sAccel.view_style > 2)
        sAccel.view_style = 0;

    // Reset current acceleration value
    sAccel.data = 0;

    // Get data from sensor
	bmp_as_get_data(sAccel.xyz);
}

// *************************************************************************************************
// @fn          init_pedo()
// @brief       Resets pedometer values
// @param
// @return
// *************************************************************************************************
void init_pedo(){
	steps = 0;
	last_readings[4] = 0;
	last_readings[3] = 0;
	last_readings[2] = 0;
	last_readings[1] = 0;
	last_readings[0] = 0;
	calibrationCounter = 0;
	sumOfRead = 0;
	calibratedValue= 0;
}

// *************************************************************************************************
// @fn          acceleration_value_is_positive
// @brief       Returns 1 if 2's complement number is positive
// @param       unsigned char value        2's complement number
// @return      unsigned char                      1 = number is positive, 0 = number is negavtive
// *************************************************************************************************
unsigned char acceleration_value_is_positive(unsigned char value)
{
    return ((value & BIT7) == 0);
}

// *************************************************************************************************
// @fn          convert_acceleration_value_to_mgrav
// @brief       Converts measured value to mgrav units
// @param       unsigned char value        g data from sensor
// @return      unsigned short                     Acceleration (mgrav)
// *************************************************************************************************
unsigned short convert_acceleration_value_to_mgrav(unsigned char value)
{
    unsigned short result;
    unsigned char i;

    if (!acceleration_value_is_positive(value))
    {
        // Convert 2's complement negative number to positive number
        value = ~value;
        value += 1;
    }

    result = 0;
    for (i = 0; i < 7; i++)
		result += ((value & (BIT(i))) >> i) * bmp_mgrav_per_bit[i];

    return (result);
}

// *************************************************************************************************
// @fn          is_acceleration_measurement
// @brief       Returns 1 if acceleration is currently measured.
// @param       none
// @return      unsigned char              1 = acceleration measurement ongoing
// *************************************************************************************************
unsigned char is_acceleration_measurement(void)
{
    return ((sAccel.mode == ACCEL_MODE_ON) && (sAccel.timeout > 0));
}

// *************************************************************************************************
// @fn          do_acceleration_measurement
// @brief       Get sensor data and store in sAccel struct
// @param       none
// @return      none
// *************************************************************************************************
void do_acceleration_measurement(void)
{
	unsigned char *str;
    // Get data from sensor
    bmp_as_get_data(sAccel.xyz);
    last_readings[4] = last_readings[3];
	last_readings[3] = last_readings[2];
	last_readings[2] = last_readings[1];
	last_readings[1] = last_readings[0];

	// Check to see if a "hump", if so, increment pedometer
	last_readings[0] = sAccel.xyz[1];
	if(calibrationCounter <= 11){
		if ((last_readings[2] < last_readings[1]) && (last_readings[1] < last_readings[0]) && (calibrationCounter <= 10))
			{
				calibrationCounter++;
				sumOfRead += last_readings[0] - last_readings[2];
			}
		else{
				calibratedValue = sumOfRead/10;
				if ((last_readings[2] < last_readings[1]) &&
					(last_readings[1] < last_readings[0]) && ((last_readings[0] - last_readings[2]) >= calibratedValue)){
					steps++;
				}
		}

	}else{
		if ((last_readings[2] < last_readings[1]) &&
			(last_readings[1] < last_readings[0]) && ((last_readings[0] - last_readings[2]) >= calibratedValue)){
			steps++;
		}
	}


}

// *************************************************************************************************
// @fn          display_acceleration
// @brief       Display routine.
// @param       unsigned char line                 LINE1
//                              unsigned char update               DISPLAY_LINE_UPDATE_FULL, DISPLAY_LINE_CLEAR
// @return      none
// *************************************************************************************************
//void display_acceleration(unsigned char line, unsigned char update)
void display_acceleration()
{
//    unsigned char *str;
    unsigned char raw_data;
//    unsigned char percent[3];
    unsigned short accel_data;
    unsigned char *str;
    unsigned long distance;

    if (!as_ok)
    {
//        display_chars(LCD_SEG_L1_2_0, (unsigned char *) "ERR", SEG_ON);

        __no_operation();
    }
    else
    {
        // Redraw whole screen
//        if (update == DISPLAY_LINE_UPDATE_FULL)
        {
            {
                // Start acceleration sensor
                if (!is_acceleration_measurement())
                {
                    // Clear previous acceleration value
                    sAccel.data = 0;

                    // Start sensor
					bmp_as_start();

                    // Set timeout counter
                    sAccel.timeout = ACCEL_MEASUREMENT_TIMEOUT;

                    // Set mode
                    sAccel.mode = ACCEL_MODE_ON;

                    // Start with Y-axis values
                    sAccel.view_style = DISPLAY_ACCEL_Y;
                }

                // Display decimal point
//                display_symbol(LCD_SEG_L1_DP1, SEG_ON);
            	__no_operation();
            }
        }

            accel_data = convert_acceleration_value_to_mgrav(raw_data) / 10;

            // Filter acceleration
            accel_data = (unsigned short) ((accel_data * 0.2) + (sAccel.data * 0.8));

            // Store average acceleration
            sAccel.data = accel_data;

            //times 2 becuz 1 swing moves 2 steps
            distance = (steps * 2) * 0.7;
			// convert to string for displaying
			str = int_to_array(distance,4,0);
			// Display
			display_chars(LCD_SEG_L1_3_0, str , SEG_ON);
        	//unsigned char *lcdmem;
            // Display sign
//            if (acceleration_value_is_positive(raw_data))
//            {
//
//                lcdmem  = (unsigned char *)0x0A20;
//                *lcdmem = (unsigned char)(*lcdmem | (BIT2));	// arrow display
//
//                __delay_cycles(500000);           // Delay ~1/2sec
//                *lcdmem = (unsigned char)(*lcdmem & (BIT2));
//                __no_operation();
//            }
//            else
//            {
//
//
//                lcdmem  = (unsigned char *)0x0A20;
//                *lcdmem = (unsigned char)(*lcdmem | (BIT3));
//
//                __delay_cycles(100000);           // Delay ~0.1sec
//                *lcdmem = (unsigned char)(*lcdmem & (BIT3));
//            	__no_operation();
//            }

    }
}

