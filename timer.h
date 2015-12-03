#ifndef BUTTONSCHRONOS_TIMER_H_
#define BUTTONSCHRONOS_TIMER_H_

// *************************************************************************************************
// Include section
#include <msp430.h>
#include "commonVar.h"

// *************************************************************************************************
// Prototypes section
extern void Timer0_Init(void);
extern void Timer0_Start(void);
extern void Timer0_Stop(void);
extern void Timer0_A3_Start(unsigned short ticks);
extern void Timer0_A3_Stop(void);
extern void Timer0_A4_Delay(unsigned short ticks);
extern void Timer1_Init(void);
extern void (*fptr_Timer0_A3_function)(void);

// *************************************************************************************************
// Defines section
struct timer
{
    // Timer0_A3 periodic delay
    unsigned short timer0_A3_ticks;
};
extern struct timer sTimer;

// Trigger reset when all buttons are pressed
#define BUTTON_RESET_SEC                (3u)

// *************************************************************************************************
// Global Variable section

// *************************************************************************************************
// Extern section

#endif                          /*BUTTONSCHRONOS_TIMER_H_ */
