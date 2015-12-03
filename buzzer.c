#include <msp430.h>
#include "display.h"
#include "timer.h"
#include <stdio.h>

#include "commonVar.h"

void init_buzzer(void) {

	P2DIR |= BIT7;                     // P2.7 output

}


