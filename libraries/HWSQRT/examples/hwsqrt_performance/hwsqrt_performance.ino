#include <HWSQRT.h>
#include <RTTStream.h>

RTTStream rtt;
HWSQRT hwsqrt;

#define	LOOPS    100000

void hardsqrtu_speed()
{
  rtt.print("hardware, integer: ");
  uint32_t sum = 0; // so nothing gets optimized away
  uint32_t t1 = millis();
  for (int i = 0; i < LOOPS; i++)
  {
    sum += hwsqrt.sqrtu(i);
  }
  uint32_t t2 = millis();
  rtt.print(t2-t1);
  rtt.println("ms");
  rtt.print("sum: ");
  rtt.println(sum);
}


void softsqrtf_speed()
{
  rtt.print("software, float: ");
  float sum = 0; // so nothing gets optimized away
  uint32_t t1 = millis();
  for (int i = 0; i < LOOPS; i++)
  {
    sum += sqrtf(i);
  }
  uint32_t t2 = millis();
  rtt.print(t2-t1);
  rtt.println("ms");
  rtt.print("sum: ");
  rtt.println(sum);
}


void setup()
{
  rtt.print("sqrt, numbers from 1 to ");
  rtt.println(LOOPS);
  hardsqrtu_speed();
  softsqrtf_speed();
}


void loop()
{
}
