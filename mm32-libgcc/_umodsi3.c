#include "reg_common.h"
#include "reg_div.h"

// return the remainder of the unsigned division of n/d
unsigned int __umodsi3(unsigned int n, unsigned int d)
{
  DIV->CR = DIV_CR_USIGN;
  DIV->DVDR = n;
  DIV->DVSR = d;               // writing DVSR automatically performs division operation
  if (d == 0) __aeabi_idiv0(); // set breakpoint on __aeabi_idiv0() to trap division by zero
  return (DIV->RMDR);
}
