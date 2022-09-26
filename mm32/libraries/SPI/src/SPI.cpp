#include <SPI.h>

extern "C" {
#include <zf_spi.h>
}

namespace arduino {
SPIClassSeekFree SPI;

SPIClassSeekFree::SPIClassSeekFree()
{
}


void SPIClassSeekFree::beginTransaction(SPISettings settings)
{
//  uint8_t mode = settings.getDataMode();
  uint8_t mode = 0;
//  uint32_t baud = settings.getClockFreq();
  uint32_t baud = 4000000;

  spi_init(SPI_2, SPI2_SCK_B13, SPI2_MOSI_B15, SPI_MISO_NULL, SPI_NSS_NULL, mode, baud);
}


void SPIClassSeekFree::endTransaction()
{
}


uint8_t SPIClassSeekFree::transfer(uint8_t data)
{
  spi_mosi(SPI_2, &data, NULL, sizeof(data));
  return (0);
}


uint16_t SPIClassSeekFree::transfer16(uint16_t data)
{
  spi_mosi(SPI_2, (uint8_t *)&data, NULL, sizeof(data));
  return (0);
}


void SPIClassSeekFree::transfer(void *buf, size_t count)
{
  spi_mosi(SPI_2, (uint8_t *)buf, NULL, count);
  return;
}


// Transaction Functions

void SPIClassSeekFree::usingInterrupt(int interruptNumber)
{
  (void)interruptNumber;
}


void SPIClassSeekFree::notUsingInterrupt(int interruptNumber)
{
  (void)interruptNumber;
}


// SPI Configuration methods
void SPIClassSeekFree::attachInterrupt()
{
}


void SPIClassSeekFree::detachInterrupt()
{
}


void SPIClassSeekFree::begin()
{
}


void SPIClassSeekFree::end()
{
}
}
