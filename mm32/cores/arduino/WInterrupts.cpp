#include "Arduino.h"
#include "WInterrupts.h"

extern "C" {
#include <zf_exti.h>
#include <isr.h>
}

/*
 * \brief Specifies a named Interrupt Service Routine (ISR) to call when an interrupt occurs.
 *        Replaces any previous function that was attached to the interrupt.
 *
 *        callback needs to be c calling convention (extern "C")
 */
void attachInterrupt(pin_size_t pin, voidFuncPtr callback, PinStatus mode)
{
  uint32_t line = pin & 0xf;
  EXTITrigger_TypeDef trigger;

  switch (mode)
  {
  case RISING:
    trigger = EXTI_Trigger_Rising;
    break;

  case FALLING:
    trigger = EXTI_Trigger_Falling;
    break;

  case CHANGE:
    trigger = EXTI_Trigger_Rising_Falling;
    break;

  default:
    trigger = EXTI_Trigger_Rising_Falling;
    break;
  }

  extisr_ptr[line] = callback;
  exti_interrupt_init((PIN_enum)pin, trigger, 0x01);
}


/*
 * \brief Turns off the given interrupt.
 */
void detachInterrupt(pin_size_t pin)
{
  uint32_t line = pin & 0xf;

  exti_interrupt_disable((PIN_enum)pin);
  extisr_ptr[line] = NULL;
}
