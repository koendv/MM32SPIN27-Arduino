#ifndef _HWSQRT_H
#define	_HWSQRT_H

#include <stdint.h>

/* MindMotion MM32. unsigned integer square root with hardware acceleration */

class HWSQRT {
public:
  HWSQRT();
  uint32_t sqrtu(uint32_t x);
};

#endif
