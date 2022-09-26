#include <HWSQRT.h>
#include <RTTStream.h>

RTTStream rtt;
HWSQRT hwsqrt;
uint32_t i;

void setup()
{
  rtt.setTimeout(300000);
  rtt.print("\nenter positive integer\n");
}


void loop()
{
  rtt.print("> ");
  i = rtt.parseInt();
  rtt.print("\nroot: ");
  rtt.println(hwsqrt.sqrtu(i));
}
