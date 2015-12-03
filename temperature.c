
#include "temperature.h"
#include "display.h"
#include "timer.h"
#include "commonVar.h"
#include "buzzer.h"
#include <stdio.h>

// *************************************************************************************************
// *************************************************************************************************
// Global Variable section
struct temp sTemp;
unsigned short adc123_result;
unsigned char adc123_data_ready;
volatile signed long old_temperature;

unsigned char temp_initFlag = 0;

// *************************************************************************************************
// Extern section

// *************************************************************************************************
// @fn          temperature_measurement
// @brief       Init ADC12. Do single conversion of temperature sensor voltage. Turn off ADC12.
// @param       none
// @return      none
// *************************************************************************************************
void temperature_measurement() {

	unsigned short adc_result;
	volatile signed long temperature;

	// Initialize the shared reference module
	REFCTL0 |= REFMSTR + REFVSEL_0 + REFON; // Enable internal reference (1.5V or 2.5V)

	// Initialize ADC12_A
	ADC12CTL0 = ADC12SHT0_8 + ADC12ON;          // Set sample time
	ADC12CTL1 = ADC12SHP;               // Enable sample timer
	ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10; // ADC input channel
	ADC12IE = 0x001;                    // ADC_IFG upon conv result-ADCMEMO

	// Wait 2 ticks (66us) to allow internal reference to settle
	Timer0_A4_Delay(2);

	// Start ADC12
	ADC12CTL0 |= ADC12ENC;

	// Clear data ready flag
	adc123_data_ready = 0;

	// Sampling and conversion start
	ADC12CTL0 |= ADC12SC;

	// Delay to get next ADC value

	Timer0_A4_Delay(5);
	while (!adc123_data_ready)
		;

	// Shut down ADC12
	ADC12CTL0 &= ~(ADC12ENC | ADC12SC | ADC12SHT0_8);
	ADC12CTL0 &= ~ADC12ON;

	// Shut down reference voltage
	REFCTL0 &= ~(REFMSTR + REFVSEL_0 + REFON);

	ADC12IE = 0;

	adc_result = adc123_result;

	temperature = (((signed long) ((signed long) adc_result - 1855)) * 667 * 10)
			/ 4096;

	// Add temperature offset
	temperature += sTemp.offset;

	// check if there is a temp drop
	if (temperature > sTemp.degrees) {

		sTemp.drop = 0;

	} else if (temperature < sTemp.degrees) {

		sTemp.drop = 1;

	}
	sTemp.degrees = (signed long) temperature;

	if (warmup_status == 0) {
		temp_initFlag = 0;
	}
	if (warmup_status == 1) {
		// Store first instance of temperature
		if (temp_initFlag == 0) {
			old_temperature = temperature;
			temp_initFlag = 1;
		}
		// Compare to old temperature to measure change in temperature
		else {
			if (temperature - old_temperature >= 10) {
				old_temperature = temperature;
				temp_initFlag = 0;
				warmup_status = 2;
			}
		}
	}

}

void display_temperature() {
	unsigned char *str;

	// Display °C
	write_lcd_mem((unsigned char *) (0x0a20), (BIT6), (BIT6), SEG_ON);
	write_lcd_mem((unsigned char *) (0x0a24), (BIT1), (BIT1), SEG_ON);
	display_char(LCD_SEG_L1_0, 'C', SEG_ON);

	temperature_measurement();

	if (sTemp.drop == 1) {
		write_lcd_mem((unsigned char *) (0x0a20), (BIT2), (BIT2), SEG_OFF);
		write_lcd_mem((unsigned char *) (0x0a20), (BIT3), (BIT3), SEG_ON);
	} else {
		write_lcd_mem((unsigned char *) (0x0a20), (BIT2), (BIT2), SEG_ON);
		write_lcd_mem((unsigned char *) (0x0a20), (BIT3), (BIT3), SEG_OFF);
	}
//	printf("%d\n", sTemp.degrees);
	str = int_to_array(sTemp.degrees, 3, 1);
//	puts(str);
	display_chars(LCD_SEG_L1_3_1, str, SEG_ON);

}

//void clear_temperature() {

//	LCDM1 = 0x00;
//	LCDM2 = 0x00;
//	LCDM3 = 0x00;
//	LCDM4 = 0x00;
	//LCDM5 = 0x00;
	//LCDM6 = 0x00;
//	LCDM7 = 0x00;
//	LCDM8 = 0x00;
//	LCDM9 = 0x00;
//	LCDM10 = 0x00;
//	LCDM11 = 0x00;
//	LCDM12 = 0x00;
//	LCDM13 = 0x00;
//	LCDM14 = 0x00;
//	LCDM15 = 0x00;
//	LCDM16 = 0x00;
//	LCDM17 = 0x00;
//	LCDM18 = 0x00;
//	LCDM19 = 0x00;
//	LCDM20 = 0x00;
//	LCDM21 = 0x00;
//	LCDM22 = 0x00;
//	LCDM23 = 0x00;


//}
// *************************************************************************************************
// @fn          ADC12ISR
// @brief       Store ADC12 conversion result. Set flag to indicate data ready.
// @param       none
// @return      none
// *************************************************************************************************
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR(void) {
	switch (__even_in_range(ADC12IV, 34)) {
	case 0:
		break;                    // Vector  0:  No interrupt
	case 2:
		break;                    // Vector  2:  ADC overflow
	case 4:
		break;                    // Vector  4:  ADC timing overflow
	case 6:                       // Vector  6:  ADC12IFG0
		adc123_result = ADC12MEM0; // Move results, IFG is cleared
		adc123_data_ready = 1;
		//_BIC_SR_IRQ(LPM3_bits);   // Exit active CPU
		break;
	case 8:
		break;                    // Vector  8:  ADC12IFG1
	case 10:
		break;                    // Vector 10:  ADC12IFG2
	case 12:
		break;                    // Vector 12:  ADC12IFG3
	case 14:
		break;                    // Vector 14:  ADC12IFG4
	case 16:
		break;                    // Vector 16:  ADC12IFG5
	case 18:
		break;                    // Vector 18:  ADC12IFG6
	case 20:
		break;                    // Vector 20:  ADC12IFG7
	case 22:
		break;                    // Vector 22:  ADC12IFG8
	case 24:
		break;                    // Vector 24:  ADC12IFG9
	case 26:
		break;                    // Vector 26:  ADC12IFG10
	case 28:
		break;                    // Vector 28:  ADC12IFG11
	case 30:
		break;                    // Vector 30:  ADC12IFG12
	case 32:
		break;                    // Vector 32:  ADC12IFG13
	case 34:
		break;                    // Vector 34:  ADC12IFG14
	default:
		break;
	}
}
