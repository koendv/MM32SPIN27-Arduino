#ifndef _VARIANT_H_
#define _VARIANT_H_

// constant with same value as SystemCoreClock variable
#define SYSTEMCORECLOCK	(48000000ul)

/* please keep arduino pin names in sync with SeekFree PIN_enum in common.h */
/* first 4 bits are bank number; second 4 bits are pin number */

#define PA0  (0x00)
#define PA1  (0x01)
#define PA2  (0x02)
#define PA3  (0x03)
#define PA4  (0x04)
#define PA5  (0x05)
#define PA6  (0x06)
#define PA7  (0x07)

#define PA8  (0x08)
#define PA9  (0x09)
#define PA10 (0x0A)
#define PA11 (0x0B)
#define PA12 (0x0C)
#define PA13 (0x0D)
#define PA14 (0x0E)
#define PA15 (0x0F)

#define PB0  (0x10)
#define PB1  (0x11)
#define PB2  (0x12)
#define PB3  (0x13)
#define PB4  (0x14)
#define PB5  (0x15)
#define PB6  (0x16)
#define PB7  (0x17)

#define PB8  (0x18)
#define PB9  (0x19)
#define PB10 (0x1A)
#define PB11 (0x1B)
#define PB12 (0x1C)
#define PB13 (0x1D)
#define PB14 (0x1E)
#define PB15 (0x1F)

#define PC0  (0x20)
#define PC1  (0x21)
#define PC2  (0x22)
#define PC3  (0x23)
#define PC4  (0x24)
#define PC5  (0x25)
#define PC6  (0x26)
#define PC7  (0x27)

#define PC8  (0x28)
#define PC9  (0x29)
#define PC10 (0x2A)
#define PC11 (0x2B)
#define PC12 (0x2C)
#define PC13 (0x2D)
#define PC14 (0x2E)
#define PC15 (0x2F)

#define PD0  (0x30)
#define PD1  (0x31)
#define PD2  (0x32)
#define PD3  (0x33)
#define PD4  (0x34)
#define PD5  (0x35)
#define PD6  (0x36)
#define PD7  (0x37)

// highest numbered pin
#define LAST_PIN PD7

// number of pins in package
#define PIN_COUNT (LAST_PIN+1)

// two dummy pin names to measure processor temperature and supply voltage
// read these dummy pins using analogRead to measure processor temperature and supply voltage
#define ATEMP (LAST_PIN+1)
#define AVS   (LAST_PIN+2)

#endif
