/*
 * commonVar.h
 *
 *  Created on: 27 Nov 2015
 *      Author: user
 */

#include <cc430x613x.h>

#ifndef COMMONVAR_H_
#define COMMONVAR_H_


// to indicate toggle flag
//extern volatile int toggle_flag;

// to indicate current mode of the watch
// 0 indicate watch is in warmup mode
// 1 indicate watch is in running mode
extern volatile char watch_mode;

// to toggle between warmup status
// 0 function status = ready
// 1 function status = active
// 2 function status = done
extern volatile char warmup_status;

// to toggle running mode
// 0 function status = pause
// 1 function status =start/resume
extern volatile char running_status;

// to indicate temperature is updated
extern volatile char temp_updated_flag;

// to indicate distance is updated
extern volatile char dist_updated_flag;

extern volatile int sec;
extern volatile int min;
extern volatile int hr;

#endif /* COMMONVAR_H_ */
