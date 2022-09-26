#include "reg_common.h"
#include "reg_div.h"

// return the quotient of the signed division of n/d
int __divsi3(int n, int d)
{
  DIV->CR = 0;
  DIV->DVDR = n;
  DIV->DVSR = d;                                        // writing DVSR automatically performs division operation
  if (d == 0) __aeabi_idiv0();                          // set breakpoint on __aeabi_idiv0() to trap division by zero
  return (DIV->QUOTR);
}


int __aeabi_idiv(int n, int d) __attribute__ ((alias("__divsi3")));

#ifdef __GNUC__
int __aeabi_divmod_ret(int quotient, int remainder);

#else
#error "Compiler not supported"
#endif

// return the quotient and remainder of the signed division of n/d
int __aeabi_idivmod(int n, int d)
{
  DIV->CR = 0;
  DIV->DVDR = n;
  DIV->DVSR = d;                                        // writing DVSR automatically performs division operation
  if (d == 0) __aeabi_idiv0();                          // set breakpoint on __aeabi_idiv0() to trap division by zero
  return (__aeabi_divmod_ret(DIV->QUOTR, DIV->RMDR));   // returns quotient in R0 and remainder in R1 by tail calling
}
