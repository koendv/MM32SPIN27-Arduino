#include <HWCRC.h>
#include <RTTStream.h>

RTTStream rtt;
HWCRC hwcrc;
uint32_t i;

#define	LEN    256

uint32_t dta[LEN];

void setup()
{
  for (int i = 0; i < LEN; i++)
  {
    dta[i] = i;
  }
  hwcrc.reset();
  hwcrc.add(dta, LEN);
  uint32_t crc32 = hwcrc.get();
  rtt.print("\ncrc32: 0x");
  rtt.print(crc32, 16);
}


void loop()
{
}
