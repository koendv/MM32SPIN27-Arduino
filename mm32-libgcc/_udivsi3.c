#include "reg_common.h"
#include "reg_div.h"

// return the quotient of the unsigned division n/d
unsigned int __udivsi3(unsigned int n, unsigned int d)
{
  DIV->CR = DIV_CR_USIGN;
  DIV->DVDR = n;
  DIV->DVSR = d;                                        // writing DVSR automatically performs division operation
  if (d == 0) __aeabi_idiv0();                          // set breakpoint on __aeabi_idiv0() to trap division by zero
  return (DIV->QUOTR);
}


unsigned int __aeabi_uidiv(unsigned int n, unsigned int d) __attribute__ ((alias("__udivsi3")));

#ifdef __GNUC__
unsigned int __aeabi_divmod_ret(unsigned int quotient, unsigned int remainder);

#else
#error "Compiler not supported"
#endif

// return the quotient and remainder of the unsigned division n/d
unsigned int __aeabi_uidivmod(unsigned int n, unsigned int d)
{
  DIV->CR = DIV_CR_USIGN;
  DIV->DVDR = n;
  DIV->DVSR = d;                                        // writing DVSR automatically performs division operation
  if (d == 0) __aeabi_idiv0();                          // set breakpoint on __aeabi_idiv0() to trap division by zero
  return (__aeabi_divmod_ret(DIV->QUOTR, DIV->RMDR));   // returns quotient in R0 and remainder in R1 by tail calling
}
