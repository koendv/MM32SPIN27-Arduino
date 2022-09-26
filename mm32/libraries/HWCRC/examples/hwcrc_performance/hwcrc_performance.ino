/*
   program output on mm32spin27 at 96MHz:

   crc32, 1kbyte data, 1000 cycles
   hardware crc: 0x96670628 49 ms
   software crc: 0x96670628 2671 ms

*/

#include <HWCRC.h>
#include <RTTStream.h>

RTTStream rtt;
HWCRC hwcrc;

#define	LEN		256
#define	CYCLES		1000

uint32_t dta[LEN];

uint32_t softcrc(uint32_t *ptr, uint32_t len)
{
  const uint32_t dwPolynomial = 0x04C11DB7;
  uint32_t xbit;
  uint32_t data;
  uint32_t CRC32 = 0xFFFFFFFF; // initial value

  while (len--)
  {
    xbit = (uint32_t)1 << 31;
    data = *ptr++;
    for (int bits = 0; bits < 32; bits++)
    {
      if (CRC32 & 0x80000000) {
        CRC32 <<= 1;
        CRC32 ^= dwPolynomial;
      }else {
        CRC32 <<= 1;
      }
      if (data & xbit) {
        CRC32 ^= dwPolynomial;
      }
      xbit >>= 1;
    }
  }
  return (CRC32);
}


void softcrc_speedtest()
{
  uint32_t crc32;

  rtt.print("software crc");
  uint32_t t1 = millis();

  for (int i = 0; i < CYCLES; i++)
  {
    crc32 = softcrc(dta, LEN);
  }
  uint32_t t2 = millis();

  rtt.print(": 0x");
  rtt.print(crc32, 16);
  rtt.print(" ");
  rtt.print(t2-t1);
  rtt.println(" ms");
}


void hwcrc_speedtest()
{
  uint32_t crc32;

  rtt.print("hardware crc");
  uint32_t t1 = millis();

  for (int i = 0; i < CYCLES; i++)
  {
    hwcrc.reset();
    hwcrc.add((uint32_t *)dta, LEN);
    crc32 = hwcrc.get();
  }
  uint32_t t2 = millis();

  rtt.print(": 0x");
  rtt.print(crc32, 16);
  rtt.print(" ");
  rtt.print(t2-t1);
  rtt.println(" ms");
}


void setup()
{
  rtt.println("\ncrc32, 1kbyte data, 1000 cycles");
  for (int i = 0; i < LEN; i++)
  {
    dta[i] = i;
  }
  hwcrc_speedtest();
  softcrc_speedtest();
}


void loop()
{
}
