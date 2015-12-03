#include <cc430x613x.h>
#include "commonVar.h"

#ifndef DRIVER_BUTTON_H_
#define DRIVER_BUTTON_H_

#define BUTTONS_IN              (P2IN)
#define BUTTONS_OUT             (P2OUT)
#define BUTTONS_DIR             (P2DIR)
#define BUTTONS_REN             (P2REN)
#define BUTTONS_IE              (P2IE)
#define BUTTONS_IES             (P2IES)
#define BUTTONS_IFG             (P2IFG)
#define BUTTONS_IRQ_VECT2       (PORT2_VECTOR)

// Button ports
#define BUTTON_DOWN             (BIT0)
#define BUTTON_HEX              (BIT1)
#define BUTTON_STAR             (BIT2)
#define BUTTON_BACKLIGHT_PIN    (BIT3)
#define BUTTON_UP               (BIT4)
#define ALL_BUTTONS             (BUTTON_DOWN + BUTTON_HEX + BUTTON_STAR + BUTTON_BACKLIGHT_PIN + BUTTON_UP)

extern void buttons_init(void);

#endif /* DRIVER_BUTTON_H_ */
