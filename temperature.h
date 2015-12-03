
#ifndef BUTTONSCHRONOS_TEMPERATURE_H_
#define BUTTONSCHRONOS_TEMPERATURE_H_

// *************************************************************************************************
// Include section

// *************************************************************************************************
// Prototypes section
extern void temperature_measurement(void);
extern void display_temperature(void);
//extern void clear_temperature(void);

// *************************************************************************************************
// Defines section

// *************************************************************************************************
// Global Variable section
struct temp
{
    signed short degrees;                // Temperature (°C) in 2.1 format
    signed short offset;                 // User set calibration value (°C) in 2.1 format
    int drop;
};
extern struct temp sTemp;
extern unsigned short adc123_result;
extern unsigned char adc123_data_ready;
extern unsigned char temp_initFlag;
// *************************************************************************************************
// Extern section

#endif                          /*BUTTONSCHRONOS_TEMPERATURE_H_ */
