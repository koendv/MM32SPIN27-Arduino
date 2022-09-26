#include "reg_common.h"
#include "reg_div.h"

// return the remainder of the signed division of n/d
int __modsi3(int n, int d)
{
  DIV->CR = 0;
  DIV->DVDR = n;
  DIV->DVSR = d;               // writing DVSR automatically performs division operation
  if (d == 0) __aeabi_idiv0(); // set breakpoint on __aeabi_idiv0() to trap division by zero
  return (DIV->RMDR);
}
