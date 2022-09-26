#ifndef _HWCRC_H
#define	_HWCRC_H

#include <stdint.h>

/* MindMotion MM32. crc32 with hardware acceleration. 
   crc calculations are performed on 32-bit words, not byte-by-byte. */

class HWCRC {
public:
  HWCRC();
  void reset();
  void add(uint32_t data);
  void add(uint32_t *buffer, uint32_t count);
  uint32_t get();
};

#endif
