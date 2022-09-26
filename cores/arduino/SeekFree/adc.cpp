#include <Arduino.h>
#include "wiring_adc.h"

extern "C" {
#include "zf_adc.h"
#include "hal_adc.h"
}

typedef struct {
  uint8_t	pin;    // arduino pin number
  ADCN_enum	adc_n;  // ADC id
  ADCCH_enum	adc_ch; // ADC channel
} adc_info;

/* list of all adc inputs */

static const adc_info adc_inputs[] =
{
  { PA0,   ADC_1, ADC1_CH00_A00 },
  { PA1,   ADC_1, ADC1_CH01_A01 },
  { PA2,   ADC_1, ADC1_CH02_A02 },
  { PA3,   ADC_1, ADC1_CH03_A03 },
  { PA4,   ADC_1, ADC1_CH04_A04 },
  { PA5,   ADC_1, ADC1_CH05_A05 },
  { PA6,   ADC_1, ADC1_CH06_A06 },
  { PA7,   ADC_1, ADC1_CH07_A07 },
  { PB0,   ADC_1, ADC1_CH08_B00 },
  { PB1,   ADC_1, ADC1_CH09_B01 },
  { PB2,   ADC_1, ADC1_CH10_B02 },
  { PB10,  ADC_1, ADC1_CH11_B10 },
  { PA10,  ADC_2, ADC2_CH00_A10 },
  { PB15,  ADC_2, ADC2_CH01_B15 },
  { PB14,  ADC_2, ADC2_CH02_B14 },
  { PB13,  ADC_2, ADC2_CH03_B13 },
  { PA4,   ADC_2, ADC2_CH04_A04 },
  { PA5,   ADC_2, ADC2_CH05_A05 },
  { PA6,   ADC_2, ADC2_CH06_A06 },
  { PA7,   ADC_2, ADC2_CH07_A07 },
  { PB0,   ADC_2, ADC2_CH08_B00 },
  { PB1,   ADC_2, ADC2_CH09_B01 },
  { PB2,   ADC_2, ADC2_CH10_B02 },
  { PB10,  ADC_2, ADC2_CH11_B10 },
  { ATEMP, ADC_1, ADC_CHE_TEMP	},
  { AVS,   ADC_1, ADC_CHF_VS	},
};

static const uint32_t adc_sample_time[] = {
  [ADC_SAMPLE_TIME_1_5] = ADC_CFGR_SAMCTL_1_5,
  [ADC_SAMPLE_TIME_2_5] = ADC_CFGR_SAMCTL_2_5,
  [ADC_SAMPLE_TIME_3_5] = ADC_CFGR_SAMCTL_3_5,
  [ADC_SAMPLE_TIME_4_5] = ADC_CFGR_SAMCTL_4_5,
  [ADC_SAMPLE_TIME_5_5] = ADC_CFGR_SAMCTL_5_5,
  [ADC_SAMPLE_TIME_6_5] = ADC_CFGR_SAMCTL_6_5,
  [ADC_SAMPLE_TIME_7_5] = ADC_CFGR_SAMCTL_7_5,
  [ADC_SAMPLE_TIME_13_5] = ADC_CFGR_SAMCTL_13_5,
  [ADC_SAMPLE_TIME_28_5] = ADC_CFGR_SAMCTL_28_5,
  [ADC_SAMPLE_TIME_41_5] = ADC_CFGR_SAMCTL_41_5,
  [ADC_SAMPLE_TIME_55_5] = ADC_CFGR_SAMCTL_55_5,
  [ADC_SAMPLE_TIME_71_5] = ADC_CFGR_SAMCTL_71_5,
  [ADC_SAMPLE_TIME_239_5] = ADC_CFGR_SAMCTL_239_5,
};

static bool adc_pin_configured[AVS+1] = { false };
static uint8_t adc_pin_input[AVS+1] = { 0 };
static ADCRES_enum adc_resolution = ADC_12BIT;

static void adc_new_pin(uint8_t pin)
{
  if (pin > AVS) {
    return;
  }
  if (pin == ATEMP) {
    ADC_TempSensorCmd(ENABLE);
  }else if (pin == AVS) {
    ADC_VrefintCmd(ENABLE);
  }
  adc_pin_configured[pin] = false;
  adc_pin_input[pin] = 0;
  for (int i = 0; i < sizeof(adc_inputs)/sizeof(adc_inputs[0]); i++)
  {
    if (adc_inputs[i].pin == pin) {
      adc_pin_configured[pin] = true;
      adc_pin_input[pin] = i;
      return;
    }
  }
  return;
}


int analogRead(pin_size_t pin)
{
  if (pin > AVS) {
    return (-1);
  }
  if (!adc_pin_configured[pin]) {
    adc_new_pin(pin);
    if (!adc_pin_configured[pin]) {
      return (-1);
    }
    ADCN_enum n = adc_inputs[adc_pin_input[pin]].adc_n;
    ADCCH_enum ch = adc_inputs[adc_pin_input[pin]].adc_ch;
    // adc_pin_init(ch) is already done in arduino pinMode(pin, INPUT_ANALOG);
    adc_init(n, ch, adc_resolution);
  }
  ADCN_enum n = adc_inputs[adc_pin_input[pin]].adc_n;
  ADCCH_enum ch = adc_inputs[adc_pin_input[pin]].adc_ch;
  uint16_t retval = adc_convert(n, ch);
  return (retval);
}


void analogReadResolution(int bits)
{
  switch (bits)
  {
  case 8:
    adc_resolution = ADC_8BIT;
    break;

  case 9:
    adc_resolution = ADC_9BIT;
    break;

  case 10:
    adc_resolution = ADC_10BIT;
    break;

  case 11:
    adc_resolution = ADC_11BIT;
    break;

  case 12:
    adc_resolution = ADC_12BIT;
    break;

  default:
    adc_resolution = ADC_12BIT;
    break;
  }
}


/* set sample time.
 *
 * if you wish to measure a voltage with large output impedance,
 * the adc needs more time. DS_MM32SPIN2x_SC.pdf 表41.最大R AIN
 *
 * a small sample time gives a fast adc, but low input impedance.
 * to get high input impedance, increase sample time.
 *
 * Note: best use ADC_CFGR_SAMCTL_239_5 to read internal temperature sensor.
 */
void analogSampleTime(adc_sample_time_enum sample_time)
{
  uint32_t adc_sam_ctl = adc_sample_time[sample_time];
  for (int i = 0; i < sizeof(adc_index)/sizeof(adc_index[0]); i++)
  {
    adc_index[i]->ADCFG &= ~(ADC_CFGR_SAMCTL << ADC_CFGR_SAMCTL_Pos);
    adc_index[i]->ADCFG |= adc_sam_ctl << ADC_CFGR_SAMCTL_Pos;
  }
}
