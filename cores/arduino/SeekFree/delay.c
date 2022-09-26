#include "hal_tim.h"
#include "hal_rcc.h"
#include "mm32_reg.h"

#include "delay.h"
#include <assert.h>

/*
 * free-running 1MHz timer for microsecond delay
 * uses basic timer TIM14
 * reserve advanced and general timers for the user program.
 */

uint16_t volatile * const tim14_cnt = &TIM14->CNT;

void delay_init()
{
  TIM_TimeBaseInitTypeDef TIM_StructInit;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM14, ENABLE);

  TIM_StructInit.TIM_Period = 0xFFFF;
  TIM_StructInit.TIM_Prescaler = 96-1; // prescale 96 MHz to 1 MHz
  TIM_StructInit.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_StructInit.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_StructInit.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM14, &TIM_StructInit);

  TIM_ClearFlag(TIM14, TIM_FLAG_Update);

  TIM14->CNT = 0;

  TIM_Cmd(TIM14, ENABLE);
}
