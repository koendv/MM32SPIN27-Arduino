/*
 * Copyright (c) 2015 Arduino LLC.  All right reserved.
 * Written by Cristian Maglie
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "FlashStorage.h"

#define	FLASH_PAGE_SIZE    (0x00000400)

extern "C" {
#include "hal_flash.h"
}

FlashClass::FlashClass(const void *flash_addr, uint32_t size) :
  PAGE_SIZE_WORDS(FLASH_PAGE_SIZE/4),
  flash_address((volatile void *)flash_addr),
  flash_size(size)
{
}


static inline uint32_t read_unaligned_uint32(const void *data)
{
  union {
    uint32_t	u32;
    uint8_t	u8[4];
  }
  res;
  const uint8_t *d = (const uint8_t *)data;
  res.u8[0] = d[0];
  res.u8[1] = d[1];
  res.u8[2] = d[2];
  res.u8[3] = d[3];
  return (res.u32);
}


/* write data to flash */

void FlashClass::write(const volatile void *flash_ptr, const void *data, uint32_t size)
{
  uint32_t size_words = (size + 3) / 4;
  uint32_t flash_addr = (uint32_t)flash_ptr;
  const uint8_t *src_addr = (uint8_t *)data;
  uint32_t buf;
  static volatile FLASH_Status gFlashStatus = FLASH_COMPLETE;

  // Do writes in pages
  FLASH_Unlock();
  while (size_words)
  {
    // erase page
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    gFlashStatus = FLASH_ErasePage(flash_addr);
    FLASH_ClearFlag(FLASH_FLAG_EOP);
    if (gFlashStatus != FLASH_COMPLETE) {
      FLASH_Lock();
      return;
    }

    // write page, one word at a time
    uint32_t i;
    for (i = 0; i < PAGE_SIZE_WORDS && size_words; i++)
    {
      buf = read_unaligned_uint32(src_addr);
      gFlashStatus = FLASH_ProgramWord(flash_addr, buf);
      if (gFlashStatus != FLASH_COMPLETE) {
        FLASH_Lock();
        return;
      }
      flash_addr += 4;
      src_addr += 4;
      size_words--;
    }

    // zero out any remaining words
    while (i < PAGE_SIZE_WORDS)
    {
      gFlashStatus = FLASH_ProgramWord(flash_addr, 0);
      if (gFlashStatus != FLASH_COMPLETE) {
        FLASH_Lock();
        return;
      }
      flash_addr += 4;
      i++;
    }
  }
  FLASH_Lock();
}


void FlashClass::read(const volatile void *flash_ptr, void *data, uint32_t size)
{
  memcpy(data, (const void *)flash_ptr, size);
}
