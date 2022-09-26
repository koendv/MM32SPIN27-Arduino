#ifndef _SERIAL_H_
#define	_SERIAL_H_

#include <api/HardwareSerial.h>

#if 0
#define SERIAL_CONSOLE 1
#endif

namespace arduino {
class HWSerial : public HardwareSerial
{
public:
  void begin(unsigned long);
  void begin(unsigned long baudrate, uint16_t config);
  void end();
  int available(void);
  int peek(void);
  int read(void);
  void flush(void);
  size_t write(uint8_t);

  operator bool();
private:
  uint8_t nextchar;
  bool nextchar_valid;
};

#ifdef SERIAL_CONSOLE
extern HWSerial Serial;
#endif

}

#endif
