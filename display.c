#include <string.h>
#include "display.h"
#include "temperature.h"

// *************************************************************************************************
// Prototypes section
void write_lcd_mem(unsigned char * lcdmem, unsigned char bits,
		unsigned char bitmask, unsigned char state);
void display_char(unsigned char segment, unsigned char chr, unsigned char mode);
void display_chars(unsigned char segments, unsigned char * str,
		unsigned char mode);

// *************************************************************************************************
// Defines section

// *************************************************************************************************
// Global Variable section

// Table with memory bit assignment for digits "0" to "9" and characters "A" to "Z"
const unsigned char lcd_font[] = {
SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F,         // Displays "0"
		SEG_B + SEG_C,                                         // Displays "1"
		SEG_A + SEG_B + SEG_D + SEG_E + SEG_G,                 // Displays "2"
		SEG_A + SEG_B + SEG_C + SEG_D + SEG_G,                 // Displays "3"
		SEG_B + SEG_C + SEG_F + SEG_G,                         // Displays "4"
		SEG_A + SEG_C + SEG_D + SEG_F + SEG_G,                 // Displays "5"
		SEG_A + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,         // Displays "6"
		SEG_A + SEG_B + SEG_C,                                 // Displays "7"
		SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G, // Displays "8"
		SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G,         // Displays "9"
		0,                                                     // Displays " "
		0,                                                     // Displays " "
		0,                                                     // Displays " "
		0,                                                     // Displays " "
		0,                                                     // Displays " "
		SEG_D + SEG_E + SEG_G,                                 // Displays "c"
		0,                                                     // Displays " "
		SEG_A + SEG_B + SEG_C + SEG_E + SEG_F + SEG_G,         // Displays "A"
		SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,                 // Displays "b"
		SEG_A + SEG_D + SEG_E + SEG_F,                         // Displays "C"
		SEG_B + SEG_C + SEG_D + SEG_E + SEG_G,                 // Displays "d"
		SEG_A + +SEG_D + SEG_E + SEG_F + SEG_G,                // Displays "E"
		SEG_A + SEG_E + SEG_F + SEG_G,                         // Displays "F"
		//  SEG_A+      SEG_C+SEG_D+SEG_E+SEG_F+SEG_G,       // Displays "G"
		SEG_A + SEG_B + SEG_C + SEG_D + SEG_F + SEG_G,         // Displays "g"
		SEG_B + SEG_C + SEG_E + SEG_F + SEG_G,                 // Displays "H"
		SEG_E + SEG_F,                                         // Displays "I"
		SEG_A + SEG_B + SEG_C + SEG_D,                         // Displays "J"
		//              SEG_B+SEG_C+      SEG_E+SEG_F+SEG_G,     // Displays "k"
		SEG_D + SEG_E + SEG_F + SEG_G,                         // Displays "k"
		SEG_D + SEG_E + SEG_F,                                 // Displays "L"
		SEG_A + SEG_B + SEG_C + SEG_E + SEG_F,                 // Displays "M"
		SEG_C + SEG_E + SEG_G,                                 // Displays "n"
		SEG_C + SEG_D + SEG_E + SEG_G,                         // Displays "o"
		SEG_A + SEG_B + SEG_E + SEG_F + SEG_G,                 // Displays "P"
		SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F,         // Displays "Q"
		SEG_E + SEG_G,                                         // Displays "r"
		SEG_A + SEG_C + SEG_D + SEG_F + SEG_G,                 // Displays "S"
		SEG_D + SEG_E + SEG_F + SEG_G,                         // Displays "t"
		SEG_C + SEG_D + SEG_E,                                 // Displays "u"
		SEG_C + SEG_D + SEG_E,                                 // Displays "u"
		SEG_G,                                                 // Displays "-"
		SEG_B + SEG_C + +SEG_E + SEG_F + SEG_G,                // Displays "X"
		SEG_B + SEG_C + SEG_D + SEG_F + SEG_G,                 // Displays "Y"
		SEG_A + SEG_B + SEG_D + SEG_E + SEG_G,                 // Displays "Z"
		};

// Table with memory address for each display element
const unsigned char *segments_lcdmem[] = {
LCD_SYMB_AM_MEM,
LCD_SYMB_PM_MEM,
LCD_SYMB_ARROW_UP_MEM,
LCD_SYMB_ARROW_DOWN_MEM,
LCD_SYMB_PERCENT_MEM,
LCD_SYMB_TOTAL_MEM,
LCD_SYMB_AVERAGE_MEM,
LCD_SYMB_MAX_MEM,
LCD_SYMB_BATTERY_MEM,
LCD_UNIT_L1_FT_MEM,
LCD_UNIT_L1_K_MEM,
LCD_UNIT_L1_M_MEM,
LCD_UNIT_L1_I_MEM,
LCD_UNIT_L1_PER_S_MEM,
LCD_UNIT_L1_PER_H_MEM,
LCD_UNIT_L1_DEGREE_MEM,
LCD_UNIT_L2_KCAL_MEM,
LCD_UNIT_L2_KM_MEM,
LCD_UNIT_L2_MI_MEM,
LCD_ICON_HEART_MEM,
LCD_ICON_STOPWATCH_MEM,
LCD_ICON_RECORD_MEM,
LCD_ICON_ALARM_MEM,
LCD_ICON_BEEPER1_MEM,
LCD_ICON_BEEPER2_MEM,
LCD_ICON_BEEPER3_MEM,
LCD_SEG_L1_3_MEM,
LCD_SEG_L1_2_MEM,
LCD_SEG_L1_1_MEM,
LCD_SEG_L1_0_MEM,
LCD_SEG_L1_COL_MEM,
LCD_SEG_L1_DP1_MEM,
LCD_SEG_L1_DP0_MEM,
LCD_SEG_L2_5_MEM,
LCD_SEG_L2_4_MEM,
LCD_SEG_L2_3_MEM,
LCD_SEG_L2_2_MEM,
LCD_SEG_L2_1_MEM,
LCD_SEG_L2_0_MEM,
LCD_SEG_L2_COL1_MEM,
LCD_SEG_L2_COL0_MEM,
LCD_SEG_L2_DP_MEM, };

// Table with bit mask for each display element
const unsigned char segments_bitmask[] = {
LCD_SYMB_AM_MASK,
LCD_SYMB_PM_MASK,
LCD_SYMB_ARROW_UP_MASK,
LCD_SYMB_ARROW_DOWN_MASK,
LCD_SYMB_PERCENT_MASK,
LCD_SYMB_TOTAL_MASK,
LCD_SYMB_AVERAGE_MASK,
LCD_SYMB_MAX_MASK,
LCD_SYMB_BATTERY_MASK,
LCD_UNIT_L1_FT_MASK,
LCD_UNIT_L1_K_MASK,
LCD_UNIT_L1_M_MASK,
LCD_UNIT_L1_I_MASK,
LCD_UNIT_L1_PER_S_MASK,
LCD_UNIT_L1_PER_H_MASK,
LCD_UNIT_L1_DEGREE_MASK,
LCD_UNIT_L2_KCAL_MASK,
LCD_UNIT_L2_KM_MASK,
LCD_UNIT_L2_MI_MASK,
LCD_ICON_HEART_MASK,
LCD_ICON_STOPWATCH_MASK,
LCD_ICON_RECORD_MASK,
LCD_ICON_ALARM_MASK,
LCD_ICON_BEEPER1_MASK,
LCD_ICON_BEEPER2_MASK,
LCD_ICON_BEEPER3_MASK,
LCD_SEG_L1_3_MASK,
LCD_SEG_L1_2_MASK,
LCD_SEG_L1_1_MASK,
LCD_SEG_L1_0_MASK,
LCD_SEG_L1_COL_MASK,
LCD_SEG_L1_DP1_MASK,
LCD_SEG_L1_DP0_MASK,
LCD_SEG_L2_5_MASK,
LCD_SEG_L2_4_MASK,
LCD_SEG_L2_3_MASK,
LCD_SEG_L2_2_MASK,
LCD_SEG_L2_1_MASK,
LCD_SEG_L2_0_MASK,
LCD_SEG_L2_COL1_MASK,
LCD_SEG_L2_COL0_MASK,
LCD_SEG_L2_DP_MASK, };


// Display flags

// Global return string for int_to_array function
unsigned char int_to_array_str[8];

void lcd_init(void) {
	// Clear entire display memory
	LCDBMEMCTL |= LCDCLRBM + LCDCLRM;

	// LCD_FREQ = ACLK/16/8 = 256Hz
	// Frame frequency = 256Hz/4 = 64Hz, LCD mux 4, LCD on
	LCDBCTL0 = (LCDDIV0 + LCDDIV1 + LCDDIV2 + LCDDIV3) | (LCDPRE0 + LCDPRE1)
			| LCD4MUX | LCDON;

	// LCB_BLK_FREQ = ACLK/8/4096 = 1Hz
	LCDBBLKCTL = LCDBLKPRE0 | LCDBLKPRE1 | LCDBLKDIV0 | LCDBLKDIV1 | LCDBLKDIV2
			| LCDBLKMOD0;

	// I/O to COM outputs
	P5SEL |= (BIT5 | BIT6 | BIT7);
	P5DIR |= (BIT5 | BIT6 | BIT7);

	// Activate LCD output
	LCDBPCTL0 = 0xFFFF;         // Select LCD segments S0-S15
	LCDBPCTL1 = 0x00FF;         // Select LCD segments S16-S22

}

void write_lcd_mem(unsigned char * lcdmem, unsigned char bits,
		unsigned char bitmask, unsigned char state) {
	if (state == SEG_ON) {
		// Clear segments before writing
		*lcdmem = (unsigned char) (*lcdmem & ~bitmask);

		// Set visible segments
		*lcdmem = (unsigned char) (*lcdmem | bits);
	} else if (state == SEG_OFF) {
		// Clear segments
		*lcdmem = (unsigned char) (*lcdmem & ~bitmask);
	} else if (state == SEG_ON_BLINK_ON) {
		// Clear visible / blink segments before writing
		*lcdmem = (unsigned char) (*lcdmem & ~bitmask);
		*(lcdmem + 0x20) = (unsigned char) (*(lcdmem + 0x20) & ~bitmask);

		// Set visible / blink segments
		*lcdmem = (unsigned char) (*lcdmem | bits);
		*(lcdmem + 0x20) = (unsigned char) (*(lcdmem + 0x20) | bits);
	} else if (state == SEG_ON_BLINK_OFF) {
		// Clear visible segments before writing
		*lcdmem = (unsigned char) (*lcdmem & ~bitmask);

		// Set visible segments
		*lcdmem = (unsigned char) (*lcdmem | bits);

		// Clear blink segments
		*(lcdmem + 0x20) = (unsigned char) (*(lcdmem + 0x20) & ~bitmask);
	} else if (state == SEG_OFF_BLINK_OFF) {
		// Clear segments
		*lcdmem = (unsigned char) (*lcdmem & ~bitmask);

		// Clear blink segments
		*(lcdmem + 0x20) = (unsigned char) (*(lcdmem + 0x20) & ~bitmask);
	}
}

// *************************************************************************************************
// @fn          int_to_array
// @brief       Generic integer to array routine. Converts integer n to string.
//                              Default conversion result has leading zeros, e.g. "00123"
//                              Option to convert leading '0' into whitespace (blanks)
// @param       unsigned long n                   integer to convert
//                              unsigned char digits               number of digits
//                              unsigned char blanks               fill up result string with number of
// whitespaces instead of leading zeros
// @return      unsigned char                              string
// *************************************************************************************************
unsigned char *int_to_array(unsigned long n, unsigned char digits,
		unsigned char blanks) {
	unsigned char i;
	unsigned char digits1 = digits;

	// Preset result string
	memcpy(int_to_array_str, "0000000", 7);

	// Return empty string if number of digits is invalid (valid range for digits: 1-7)
	if ((digits == 0) || (digits > 7))
		return (int_to_array_str);

	// Calculate digits from least to most significant number
	do {
		int_to_array_str[digits - 1] = n % 10 + '0';
		n /= 10;
	} while (--digits > 0);

	// Remove specified number of leading '0', always keep last one
	i = 0;
	while ((int_to_array_str[i] == '0') && (i < digits1 - 1)) {
		if (blanks > 0) {
			// Convert only specified number of leading '0'
			int_to_array_str[i] = ' ';
			blanks--;
		}
		i++;
	}

	return (int_to_array_str);
}



// *************************************************************************************************
// @fn          display_char
// @brief       Write to 7-segment characters.
// @param       unsigned char segment              A valid LCD segment
//                              unsigned char chr                  Character to display
//                              unsigned char mode         SEG_ON, SEG_OFF, SEG_BLINK
// @return      none
// *************************************************************************************************
void display_char(unsigned char segment, unsigned char chr, unsigned char mode) {
	unsigned char *lcdmem;                 // Pointer to LCD memory
	unsigned char bitmask;                 // Bitmask for character
	unsigned char bits, bits1;             // Bits to write

	// Write to single 7-segment character
	if ((segment >= LCD_SEG_L1_3) && (segment <= LCD_SEG_L2_DP)) {
		// Get LCD memory address for segment from table
		lcdmem = (unsigned char *) segments_lcdmem[segment];

		// Get bitmask for character from table
		bitmask = segments_bitmask[segment];

		// Get bits from font set
		if ((chr >= 0x30) && (chr <= 0x5A)) {
			// Use font set
			bits = lcd_font[chr - 0x30];
		} else if (chr == 0x2D) {
			// '-' not in font set
			bits = BIT1;
		} else {
			// Other characters map to ' ' (blank)
			bits = 0;
		}

		// When addressing LINE2 7-segment characters need to swap high- and low-nibble,
		// because LCD COM/SEG assignment is mirrored against LINE1
		if (segment >= LCD_SEG_L2_5) {
			bits1 = ((bits << 4) & 0xF0) | ((bits >> 4) & 0x0F);
			bits = bits1;

			// When addressing LCD_SEG_L2_5, need to convert ASCII '1' and 'L' to 1 bit,
			// because LCD COM/SEG assignment is special for this incomplete character
			if (segment == LCD_SEG_L2_5) {
				if ((chr == '1') || (chr == 'L'))
					bits = BIT7;
			}
		}

		// Physically write to LCD memory
		write_lcd_mem(lcdmem, bits, bitmask, mode);
	}
}

// *************************************************************************************************
// @fn          display_chars
// @brief       Write to consecutive 7-segment characters.
// @param       unsigned char segments     LCD segment array
//                              unsigned char * str                Pointer to a string
//                              unsigned char mode             SEG_ON, SEG_OFF, SEG_BLINK
// @return      none
// *************************************************************************************************
void display_chars(unsigned char segments, unsigned char * str,
		unsigned char mode) {
	unsigned char i;
	unsigned char length = 0;              // Write length
	unsigned char char_start;            // Starting point for consecutive write

	switch (segments) {
	// LINE1
	case LCD_SEG_L1_3_0:
		length = 4;
		char_start = LCD_SEG_L1_3;
		break;
	case LCD_SEG_L1_2_0:
		length = 3;
		char_start = LCD_SEG_L1_2;
		break;
	case LCD_SEG_L1_1_0:
		length = 2;
		char_start = LCD_SEG_L1_1;
		break;
	case LCD_SEG_L1_3_1:
		length = 3;
		char_start = LCD_SEG_L1_3;
		break;
	case LCD_SEG_L1_3_2:
		length = 2;
		char_start = LCD_SEG_L1_3;
		break;

		// LINE2
	case LCD_SEG_L2_5_0:
		length = 6;
		char_start = LCD_SEG_L2_5;
		break;
	case LCD_SEG_L2_4_0:
		length = 5;
		char_start = LCD_SEG_L2_4;
		break;
	case LCD_SEG_L2_3_0:
		length = 4;
		char_start = LCD_SEG_L2_3;
		break;
	case LCD_SEG_L2_2_0:
		length = 3;
		char_start = LCD_SEG_L2_2;
		break;
	case LCD_SEG_L2_1_0:
		length = 2;
		char_start = LCD_SEG_L2_1;
		break;
	case LCD_SEG_L2_5_4:
		length = 2;
		char_start = LCD_SEG_L2_5;
		break;
	case LCD_SEG_L2_5_2:
		length = 4;
		char_start = LCD_SEG_L2_5;
		break;
	case LCD_SEG_L2_3_2:
		length = 2;
		char_start = LCD_SEG_L2_3;
		break;
	case LCD_SEG_L2_4_2:
		length = 3;
		char_start = LCD_SEG_L2_4;
		break;
	}

	// Write to consecutive digits
	for (i = 0; i < length; i++) {
		// Use single character routine to write display memory
		display_char(char_start + i, *(str + i), mode);
	}
}

void clear_display() {
	// Clear entire display memory
	LCDBMEMCTL |= LCDCLRBM + LCDCLRM;
}
