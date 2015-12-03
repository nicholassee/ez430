#include <msp430.h>
#include <stdio.h>
#include "display.h"
#include "button.h"
#include "commonVar.h"
#include "timer.h"
#include "acceleration.h"

#define CONV_MS_TO_TICKS(msec)                          (((msec) * 32768) / 1000)

#ifndef DRIVER_BUTTON_C_
#define DRIVER_BUTTON_C_

void buttons_init(void) {
	// Set button ports to input
	BUTTONS_DIR &= ~ALL_BUTTONS;
	// Enable internal pull-downs
	BUTTONS_OUT &= ~ALL_BUTTONS;
	BUTTONS_REN |= ALL_BUTTONS;
	// IRQ triggers on rising edge
	BUTTONS_IES &= ~ALL_BUTTONS;
	// Reset IRQ flags
	BUTTONS_IFG &= ~ALL_BUTTONS;
	// Enable button interrupts
	BUTTONS_IE |= ALL_BUTTONS;
}

#endif /* DRIVER_BUTTON_C_ */

#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void) {

	switch (__even_in_range(P2IV, 10)) {

	case 0x06: //BUTTON_STAR
		if (watch_mode == 0) {
			if (warmup_status == 0) { //currently warmup mode, ready
				warmup_status = 1; //toggle warmup to active
			} else if (warmup_status == 1) { //currently warmup mode, active
				warmup_status = 0; //toggle warmup to be ready
			}

		} else if (watch_mode == 1) { //running mode
			if (running_status == 0) { //timer paused
				running_status = 1; //start/resume running timer
			} else if (running_status == 1) { //timer started/resumed
				running_status = 0; //pause running timer
			}
		}
		Timer0_A4_Delay(327); //~100ms
		break;

	case 0x02: //BUTTON_DWN
		//warmup mode
		__no_operation();
		if (watch_mode == 0) {
			if (warmup_status == 0) { //currently warmup mode, ready
				warmup_status = 1; //toggle warmup to active
			} else if (warmup_status == 1) { //currently warmup mode, active
				warmup_status = 0; //toggle warmup to be ready
			} else if (warmup_status == 2){
				warmup_status = 0;
			}
		} else if (watch_mode == 1) { //running mode
			__no_operation();
			if (running_status == 0) { //timer paused
				running_status = 1; //start/resume running timer
			} else if (running_status == 1) { //timer started/resumed
				running_status = 0; //pause running timer
			}

		}
		__no_operation();
		Timer0_A4_Delay(327); //~100ms
		break;

	case 0x04: //BUTTON_HEX
		if (watch_mode == 0) { //in warmup mode
			break;
		} else if (watch_mode == 1) { //in running mode
			sec = 0;
			min = 0;
			hr = 0;
			running_status = 0; //timer stop
			init_pedo();
		}
		Timer0_A4_Delay(327); //~100ms
		break;

	case 0x08: //BUTTON_BACKLIGHT

		break;

	case 0x0A: //BUTTON_UP
		clear_display();
		__no_operation();
		if (watch_mode == 0) { //in warmup mode
			watch_mode = 1; //toggle to exercise mode
			sec = 0;
			min = 0;
			hr = 0;
			running_status = 0;
		} else if (watch_mode == 1) { //in exercise mode
			watch_mode = 0; //toggle to warmup mode
		}
		Timer0_A4_Delay(327); //~100ms
		break;

	default:
		break;

	}
}
