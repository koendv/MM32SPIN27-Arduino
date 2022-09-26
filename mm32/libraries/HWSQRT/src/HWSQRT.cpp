#include "HWSQRT.h"

extern "C" {
#include "hal_rcc.h"
#include "reg_common.h"
#include "reg_root.h"
}

HWSQRT::HWSQRT()
{
  /* start HWSQRT device */
  RCC_AHBPeriphClockCmd(RCC_AHBENR_HWSQRT, ENABLE);
}


uint32_t HWSQRT::sqrtu(uint32_t x)
{
  SQRT->SQR = x;
  return (SQRT->RESULT);
}
