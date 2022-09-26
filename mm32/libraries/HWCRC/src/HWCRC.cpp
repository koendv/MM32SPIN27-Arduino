#include "HWCRC.h"

extern "C" {
#include "hal_rcc.h"
#include "reg_common.h"
#include "reg_crc.h"
}

HWCRC::HWCRC()
{
  RCC_AHBPeriphClockCmd(RCC_AHBENR_CRC, ENABLE);
}


void HWCRC::reset()
{
  CRC->CR = CRC_CR_RESET;
}


void HWCRC::add(uint32_t data)
{
  CRC->DR = data;
}


void HWCRC::add(uint32_t *buffer, uint32_t count)
{
  for (uint32_t i = 0; i < count; i++)
  {
    CRC->DR = *buffer++;
  }
}


uint32_t HWCRC::get()
{
  return (CRC->DR);
}
