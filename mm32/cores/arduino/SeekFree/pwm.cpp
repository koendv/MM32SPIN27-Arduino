#include "Common.h"
#include "wiring_pwm.h"

extern "C" {
#include "zf_pwm.h"
#include "variant.h"
}

#if PWM_DUTY_MAX == 50000
#error first patch zf_pwm.c
#endif

typedef struct {
  PIN_enum		pin;
  TIM_enum		tim;
  TIM_PWMPIN_enum	chan;
} tim_info;

/* list of all timer pwm channels */

static const tim_info pin2tim[] = {
#if 0
/* TIM_14 is used for delayMicroseconds() */
  { A0, TIM_14, TIM_14_CH1_A00  },
  { A4,	 TIM_14, TIM_14_CH1_A04 },
  { A7,	 TIM_14, TIM_14_CH1_A07 },
  { B1,	 TIM_14, TIM_14_CH1_B01 },
#endif
  { A6, TIM_16, TIM_16_CH1_A06  },
  { B8,	 TIM_16, TIM_16_CH1_B08 },
  { A7,	 TIM_17, TIM_17_CH1_A07 },
  { B9,	 TIM_17, TIM_17_CH1_B09 },
  { B13, TIM_17, TIM_17_CH1_B13 },
  { D7,	 TIM_17, TIM_17_CH1_D07 },
  { A8,	 TIM_1,	 TIM_1_CH1_A08	},
  { B5,	 TIM_1,	 TIM_1_CH1_B05	},
  { B7,	 TIM_1,	 TIM_1_CH1_B07	},
  { B8,	 TIM_1,	 TIM_1_CH1_B08	},
  { D6,	 TIM_1,	 TIM_1_CH1_D06	},
  { A1,	 TIM_1,	 TIM_1_CH2_A01	},
  { A9,	 TIM_1,	 TIM_1_CH2_A09	},
  { B5,	 TIM_1,	 TIM_1_CH2_B05	},
  { B6,	 TIM_1,	 TIM_1_CH2_B06	},
  { A3,	 TIM_1,	 TIM_1_CH3_A03	},
  { A10, TIM_1,	 TIM_1_CH3_A10	},
  { A11, TIM_1,	 TIM_1_CH3_A11	},
  { B3,	 TIM_1,	 TIM_1_CH3_B03	},
  { B7,	 TIM_1,	 TIM_1_CH3_B07	},
  { A11, TIM_1,	 TIM_1_CH4_A11	},
  { A0,	 TIM_2,	 TIM_2_CH1_A00	},
  { A5,	 TIM_2,	 TIM_2_CH1_A05	},
  { A11, TIM_2,	 TIM_2_CH1_A11	},
  { A15, TIM_2,	 TIM_2_CH1_A15	},
  { C7,	 TIM_2,	 TIM_2_CH1_C07	},
  { C13, TIM_2,	 TIM_2_CH1_C13	},
  { A1,	 TIM_2,	 TIM_2_CH2_A01	},
  { A12, TIM_2,	 TIM_2_CH2_A12	},
  { B3,	 TIM_2,	 TIM_2_CH2_B03	},
  { C8,	 TIM_2,	 TIM_2_CH2_C08	},
  { C14, TIM_2,	 TIM_2_CH2_C14	},
  { A2,	 TIM_2,	 TIM_2_CH3_A02	},
  { B1,	 TIM_2,	 TIM_2_CH3_B01	},
  { B10, TIM_2,	 TIM_2_CH3_B10	},
  { C9,	 TIM_2,	 TIM_2_CH3_C09	},
  { C15, TIM_2,	 TIM_2_CH3_C15	},
  { A3,	 TIM_2,	 TIM_2_CH4_A03	},
  { B11, TIM_2,	 TIM_2_CH4_B11	},
  { A6,	 TIM_3,	 TIM_3_CH1_A06	},
  { B4,	 TIM_3,	 TIM_3_CH1_B04	},
  { C4,	 TIM_3,	 TIM_3_CH1_C04	},
  { C6,	 TIM_3,	 TIM_3_CH1_C06	},
  { D7,	 TIM_3,	 TIM_3_CH1_D07	},
  { A7,	 TIM_3,	 TIM_3_CH2_A07	},
  { B5,	 TIM_3,	 TIM_3_CH2_B05	},
  { B8,	 TIM_3,	 TIM_3_CH2_B08	},
  { C5,	 TIM_3,	 TIM_3_CH2_C05	},
  { C7,	 TIM_3,	 TIM_3_CH2_C07	},
  { B0,	 TIM_3,	 TIM_3_CH3_B00	},
  { B9,	 TIM_3,	 TIM_3_CH3_B09	},
  { C6,	 TIM_3,	 TIM_3_CH3_C06	},
  { C8,	 TIM_3,	 TIM_3_CH3_C08	},
  { B1,	 TIM_3,	 TIM_3_CH4_B01	},
  { C9,	 TIM_3,	 TIM_3_CH4_C09	},
  { C0,	 TIM_8,	 TIM_8_CH1_C00	},
  { C6,	 TIM_8,	 TIM_8_CH1_C06	},
  { C1,	 TIM_8,	 TIM_8_CH2_C01	},
  { C2,	 TIM_8,	 TIM_8_CH2_C02	},
  { C7,	 TIM_8,	 TIM_8_CH2_C07	},
  { C2,	 TIM_8,	 TIM_8_CH3_C02	},
  { C8,	 TIM_8,	 TIM_8_CH3_C08	},
  { D4,	 TIM_8,	 TIM_8_CH3_D04	},
  { C9,	 TIM_8,	 TIM_8_CH4_C09	},
};

static const TIM_enum pwm_timer[] = {
  [PWM_TIM_1] = TIM_1,
  [PWM_TIM_2] = TIM_2,
  [PWM_TIM_3] = TIM_3,
  [PWM_TIM_8] = TIM_8,
  [PWM_TIM_14] = TIM_14,
  [PWM_TIM_16] = TIM_16,
  [PWM_TIM_17] = TIM_17,
};

static bool pwm_configured[PIN_COUNT] = { 0 };
static int16_t pwm_channel[PIN_COUNT];
static uint32_t pwm_frequency = PWM_FREQUENCY;
static uint32_t pwm_resolution = 12;

// Set the frequency of analogWrite. Default is PWM_FREQUENCY (1000) in Hertz.
void analogWriteFrequency(uint32_t freq)
{
  pwm_frequency = freq;
}


// Set the resolution of analogWrite parameters. Default is 8 bits (range from 0 to 255)
void analogWriteResolution(int res)
{
  if ((res >= 8) && (res <= 15))
    pwm_resolution = res;
}


// Writes an analog value (PWM wave) to a pin.
void analogWrite(pin_size_t pinNumber, int value)
{
  PIN_enum zf_pin = (PIN_enum)pinNumber;

  if (!pwm_configured[zf_pin]) {
    /* first time, initialize pwm */
    const uint32_t pwm_channel_max = sizeof(pin2tim)/sizeof(pin2tim[0]);
    for (int i = 0; i < pwm_channel_max; i++)
    {
      if (pin2tim[i].pin == zf_pin) {
        /* configure pwm on pin */
        pwm_configured[zf_pin] = true;
        pwm_channel[zf_pin] = i;
        TIM_enum tim = pin2tim[pwm_channel[zf_pin]].tim;
        TIM_PWMPIN_enum chan = pin2tim[pwm_channel[zf_pin]].chan;
        pwm_init(tim, chan, pwm_frequency, value);
        return;
      }
    }
    /* pin does not have pwm */
    return;
  }

  /* update duty cycle */
  TIM_enum tim = pin2tim[pwm_channel[zf_pin]].tim;
  TIM_PWMPIN_enum chan = pin2tim[pwm_channel[zf_pin]].chan;
  uint32_t duty = value << (15 - pwm_resolution);

  pwm_duty_updata(tim, chan, duty);
}


// Writes an analog value (PWM wave) to a pin, using a specified timer
void analogWrite(pin_size_t pinNumber, int value, pwm_timer_enum pwm_tim)
{
  PIN_enum zf_pin = (PIN_enum)pinNumber;
  TIM_enum zf_tim = pwm_timer[pwm_tim];

  if (!pwm_configured[zf_pin]) {
    /* first time, initialize pwm */
    const uint32_t pwm_channel_max = sizeof(pin2tim)/sizeof(pin2tim[0]);
    for (int i = 0; i < pwm_channel_max; i++)
    {
      if ((pin2tim[i].pin == zf_pin) && (pin2tim[i].tim == zf_tim)) {
        /* configure pwm on pin */
        pwm_configured[zf_pin] = true;
        pwm_channel[zf_pin] = i;
        TIM_enum tim = pin2tim[pwm_channel[zf_pin]].tim;
        TIM_PWMPIN_enum chan = pin2tim[pwm_channel[zf_pin]].chan;
        pwm_init(tim, chan, pwm_frequency, value);
        return;
      }
    }
    /* pin does not have pwm */
    return;
  }

  /* update duty cycle */
  TIM_enum tim = pin2tim[pwm_channel[zf_pin]].tim;
  TIM_PWMPIN_enum pin = pin2tim[pwm_channel[zf_pin]].chan;
  uint32_t duty = value << (15 - pwm_resolution);

  pwm_duty_updata(tim, pin, duty);
}
