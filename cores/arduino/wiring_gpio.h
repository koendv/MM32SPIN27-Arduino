#ifndef _WIRING_GPIO_H_
#define	_WIRING_GPIO_H_


// Configures the specified pin to behave either as an input or an output.
extern void pinMode(pin_size_t pinNumber, PinMode pinMode);

// Write a HIGH or a LOW value to a digital pin.
extern void digitalWrite(pin_size_t pinNumber, PinStatus status);

// Reads the value from a specified digital pin, either HIGH or LOW.
extern PinStatus digitalRead(pin_size_t pinNumber);

// Toggle the value from a specified digital pin.
extern void digitalToggle(pin_size_t pinNumber);

#endif
