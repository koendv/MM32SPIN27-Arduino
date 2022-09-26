#include <Serial.h>

/* basic serial.
 * tx on pin b6, rx on pin b7 */

extern "C" {
#include <zf_uart.h>
}

namespace arduino {
void HWSerial::begin(unsigned long baudrate)
{
  uint32_t baud = baudrate;
  uart_init(UART_1, baud, UART1_TX_B06, UART1_RX_B07);
  nextchar_valid = false;
}


void HWSerial::begin(unsigned long baudrate, uint16_t config)
{
  (void)config;
  begin(baudrate);
}


void HWSerial::end()
{
}


int HWSerial::available(void)
{
  if (nextchar_valid) {
    return (1);
  }
  nextchar_valid = uart_query(UART_1, &nextchar);
  if (nextchar_valid) {
    return (1);
  }
  return (0);
}


int HWSerial::peek(void)
{
  if (nextchar_valid) {
    return (nextchar);
  }
  nextchar_valid = uart_query(UART_1, &nextchar);
  if (nextchar_valid) {
    return (nextchar);
  }
  return (-1);
}


int HWSerial::read(void)
{
  if (nextchar_valid) {
    nextchar_valid = false;
    return (nextchar);
  }
  if (uart_query(UART_1, &nextchar)) {
    return (nextchar);
  }
  return (-1);
}


void HWSerial::flush(void)
{
}


size_t HWSerial::write(uint8_t ch)
{
  uart_putchar(UART_1, ch);
}


HWSerial::operator bool()
{
  return (true);
}

#ifdef SERIAL_CONSOLE
HWSerial Serial;
#endif

}
