#ifndef _ARDUINO_H_
#define _ARDUINO_H_

#include "ArduinoAPI.h"
#include "variant.h"
#include "WMath.h"
#include "delay.h"
#include "itoa.h"

#include "wiring_adc.h"
#include "wiring_gpio.h"
#include "wiring_pwm.h"

extern int main(void);
extern void setup(void);
extern void loop(void);

#define interrupts() { __asm volatile ("cpsie i" : : : "memory"); }
#define noInterrupts() { __asm volatile ("cpsid i" : : : "memory"); }

using namespace arduino;

#endif
