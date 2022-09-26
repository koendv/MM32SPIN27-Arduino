#ifndef _WIRING_PWM_H_
#define	_WIRING_PWM_H_

// default pwm frequency
#define	PWM_FREQUENCY    1000

#define MAX_PWM_RESOLUTION 15

typedef enum pwm_timer_enum { PWM_TIM_1, PWM_TIM_2, PWM_TIM_3, PWM_TIM_8, PWM_TIM_14, PWM_TIM_16, PWM_TIM_17 };

// Set the resolution of analogWrite parameters. Default is 8 bits (range from 0 to 255)
extern void analogWriteResolution(int res);

// Set the frequency of analogWrite. Default is PWM_FREQUENCY (1000) in Hertz.
extern void analogWriteFrequency(uint32_t freq);

// Writes an analog value (PWM wave) to a pin.
void analogWrite(pin_size_t pinNumber, int value);

// Writes an analog value (PWM wave) to a pin, using a hardware timer.
void analogWrite(pin_size_t pinNumber, int value, pwm_timer_enum pwm_tim);

#endif
