#ifndef _DELAY_H_
#define	_DELAY_H_

void delay_init();

/* delay functions are inline for accuracy */

extern uint16_t volatile * const tim14_cnt;

void inline delay(unsigned long ms)
{
  while (ms)
  {
    *tim14_cnt = 0;
    --ms;
    while (*tim14_cnt < 1000)
    {
    }
  }
}


void inline delayMicroseconds(unsigned int us)
{
  *tim14_cnt = 0;
  while (*tim14_cnt < us)
  {
  }
}


#endif
