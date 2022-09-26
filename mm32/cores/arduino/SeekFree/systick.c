#include "Common.h"
#include "delay.h"

#include "zf_systick.h"
#include "mm32_reg.h"

// for SYSTEMCORECLOCK system clock frequency in Hz
#include "variant.h"

static volatile uint32_t systick_count = 0;

void init() {
	systick_timing_ms(1);
	delay_init(); // 1 MHz timer for delay() and delayMicroseconds()

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_HWDIV, ENABLE);  // clock for hardware divide
}

void SysTick_Handler() {
	systick_count++;
}

unsigned long millis(void) {
	return systick_count;
}

// Interrupt-compatible version of micros
// Theory: repeatedly take readings of SysTick counter, millis counter and SysTick interrupt pending flag.
// When it appears that millis counter and pending is stable and SysTick hasn't rolled over, use these
// values to calculate micros. If there is a pending SysTick, add one to the millis counter in the calculation.
unsigned long micros( void )
{
  uint32_t ticks, ticks2;
  uint32_t pend, pend2;
  uint32_t count, count2;

  ticks2  = SysTick->VAL;
  pend2   = !!(SCB->ICSR & SCB_ICSR_PENDSTSET_Msk)  ;
  count2  = systick_count ;

  do
  {
    ticks=ticks2;
    pend=pend2;
    count=count2;
    ticks2  = SysTick->VAL;
    pend2   = !!(SCB->ICSR & SCB_ICSR_PENDSTSET_Msk)  ;
    count2  = systick_count ;
  } while ((pend != pend2) || (count != count2) || (ticks < ticks2));

  return ((count+pend) * 1000) + (((SysTick->LOAD  - ticks)*(1048576/(SYSTEMCORECLOCK/1000000)))>>20) ;
  // this is an optimization to turn a runtime division into two compile-time divisions and
  // a runtime multiplication and shift, saving a few cycles
}
