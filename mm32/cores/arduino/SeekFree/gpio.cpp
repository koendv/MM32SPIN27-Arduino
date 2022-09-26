#include "Common.h"
#include "wiring_gpio.h"

extern "C" {
#include "zf_gpio.h"
#include "variant.h"
}

static bool pinInitialized[PIN_COUNT] = { 0 };

void pinMode(pin_size_t pinNumber, PinMode pinMode)
{
  if (!pinInitialized[pinNumber]) {
    switch (pinMode)
    {
    case INPUT:
      gpio_init((PIN_enum)pinNumber, GPI, GPIO_LOW, GPI_FLOATING_IN);
      break;

    case INPUT_PULLUP:
      gpio_init((PIN_enum)pinNumber, GPI, GPIO_LOW, GPI_PULL_UP);
      break;

    case INPUT_PULLDOWN:
      gpio_init((PIN_enum)pinNumber, GPI, GPIO_LOW, GPI_PULL_DOWN);
      break;

    case INPUT_ANALOG:
      gpio_init((PIN_enum)pinNumber, GPI, GPIO_LOW, GPI_ANAOG_IN);
      break;

    case OUTPUT:
      gpio_init((PIN_enum)pinNumber, GPO, GPIO_LOW, GPO_PUSH_PULL);
      break;

    case OUTPUT_OPENDRAIN:
      gpio_init((PIN_enum)pinNumber, GPO, GPIO_LOW, GPO_OPEN_DTAIN);
      break;

    default:
      break;
    }
    pinInitialized[pinNumber] = true;
  }  else{
    switch (pinMode)
    {
    case INPUT:
      gpio_dir((PIN_enum)pinNumber, GPI, GPI_FLOATING_IN);
      break;

    case INPUT_PULLUP:
      gpio_dir((PIN_enum)pinNumber, GPI, GPI_PULL_UP);
      break;

    case INPUT_PULLDOWN:
      gpio_dir((PIN_enum)pinNumber, GPI, GPI_PULL_DOWN);
      break;

    case OUTPUT:
      gpio_dir((PIN_enum)pinNumber, GPO, GPO_PUSH_PULL);
      break;

    case OUTPUT_OPENDRAIN:
      gpio_dir((PIN_enum)pinNumber, GPO, GPO_OPEN_DTAIN);
      break;

    default:
      break;
    }
  }
}


void digitalWrite(pin_size_t pinNumber, PinStatus status)
{
  gpio_set((PIN_enum)pinNumber, status);
}


PinStatus digitalRead(pin_size_t pinNumber)
{
  return ((PinStatus)gpio_get((PIN_enum)pinNumber));
}


void digitalToggle(pin_size_t pinNumber)
{
  gpio_toggle((PIN_enum)pinNumber);
}
