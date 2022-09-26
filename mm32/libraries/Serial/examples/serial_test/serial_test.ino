/* hardware serial */

#include <Serial.h>
HWSerial Serial;

void setup()
{
  Serial.begin(9600);
  Serial.println("type any key");
}


void loop()
{
  if (Serial.available()) {
    char ch;
    ch = Serial.read();
    Serial.print(ch);
  }
}
