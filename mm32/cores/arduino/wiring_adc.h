#ifndef _WIRING_ADC_H_
#define	_WIRING_ADC_H_

typedef enum adc_sample_time_enum {
  ADC_SAMPLE_TIME_1_5,
  ADC_SAMPLE_TIME_2_5,
  ADC_SAMPLE_TIME_3_5,
  ADC_SAMPLE_TIME_4_5,
  ADC_SAMPLE_TIME_5_5,
  ADC_SAMPLE_TIME_6_5,
  ADC_SAMPLE_TIME_7_5,
  ADC_SAMPLE_TIME_13_5,
  ADC_SAMPLE_TIME_28_5,
  ADC_SAMPLE_TIME_41_5,
  ADC_SAMPLE_TIME_55_5,
  ADC_SAMPLE_TIME_71_5,
  ADC_SAMPLE_TIME_239_5
};

// ADC. Reads the value from the specified analog pin
extern int analogRead(pin_size_t pin);

// ADC. Set the resolution of analogRead return values. Default is 10 bits
extern void analogReadResolution(int bits);

// ADC. Set sample time (and corresponding input impedance)
extern void analogSampleTime(adc_sample_time_enum sample_time);

#endif
