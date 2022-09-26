
// Signal differences
// ------------------
//    When the AVR microcontroller is set into hardware I2C mode,
//    the pins follow the I2C specifications for voltage levels and current.
//    For example the current to pull SDA or SCL low is maximum 3mA.
//
//    With the Software I2C, a normal pin is used which can sink/source 40mA for a ATmega328P.
//    That could increase the voltage spikes and could increase interference between sda and scl.
//    The voltage levels are different.
//    The timing of the software I2C is also different.
//
//    In most cases the software I2C should work.
//    With longer wires or with non-matching voltage levels, the result is unpredictable.
//
//
//
// Clock pulse stretching
// ----------------------
//    An I2C Slave could stretch the clock signal by keeping the SCL low.
//    This happens for example when a Slave is an Arduino which can be
//    busy doing other things like handling other interrupts.
//    Adding a check for the clock stretching should make the transmission
//    completely reliable without any loss.
//    Only an Arduino as Slave would do clock stretching, normal devices
//    like sensors and I2C EEPROM don't use clock stretching.
//    The extra check for clock stretching slows down the transfer rate.
//
//    Using millis() instead of micros() is faster.
//    That is why millis() is used for the timeout of the clock pulse stretching.
//
//
//
// Arduino Stream class
// --------------------
//    The Arduino Stream class is used by many Arduino objects.
//    For the I2C bus, the benefits are less obvious.
//    For example the parseInt() function is not used with I2C.
//    At this moment the Stream class is not used.
//
//
// Multiple Slaves with the same I2C address
// -----------------------------------------
//    The SoftwareWire can be declared more than once,
//    to create a number of software i2c busses.
//    That makes it possible to use a number of I2C devices,
//    which have the same I2C address.
//    Every software i2c bus requires 2 pins,
//    and every SoftwareWire object requires 59 bytes at the moment.
//



//  added code to i2c_stop(), since a problem was reported here:
//  http://forum.arduino.cc/index.php?topic=348337.0
//  Added lines have keyword "ADDED1".


// Use the next define to run a i2c_scanner inside the printStatus() function.
// #define ENABLE_I2C_SCANNER

#include "SoftwareWire.h"

/* mm32: 
   - direct gpio register access for speed
   assumption:
   - external pullups used
   - scl and sda same gpio bank
   - scl and sda same crl/crh register
 */



extern "C" {
#include "zf_gpio.h"
}

// Sets SDA low and drives output.

#define i2c_sda_lo()              \
{ \
  if (_scl_val) *_i2c_cr = _cr_sda_lo_scl_hi; \
  else *_i2c_cr = _cr_sda_lo_scl_lo; \
  _sda_val = false; \
}


// sets SCL low and drives output.
#define i2c_scl_lo()              \
{ \
  if (_sda_val) *_i2c_cr = _cr_sda_hi_scl_lo; \
  else *_i2c_cr = _cr_sda_lo_scl_lo; \
  _scl_val = false; \
}


// Set SDA high and to input (releases pin)
#define i2c_sda_hi()              \
{ \
  if (_scl_val) *_i2c_cr = _cr_sda_hi_scl_hi; \
  else *_i2c_cr = _cr_sda_hi_scl_lo; \
  _sda_val = true; \
}


// set SCL high and to input (releases pin)
#define i2c_scl_hi()              \
{ \
  if (_sda_val) *_i2c_cr = _cr_sda_hi_scl_hi; \
  else *_i2c_cr = _cr_sda_lo_scl_hi; \
  _scl_val = true; \
}


// Read the bit value of the pin
// Note that is the pin can also be read when it is an output.
#define i2c_sda_read()   ((*_i2c_idr & _idr_sda_mask) ? 1 : 0)
#define i2c_scl_read()   ((*_i2c_idr & _idr_scl_mask) ? 1 : 0)

static void i2c_bad_pins()
{
  // hang if sda and scl pins do not have same idr and cr
  while(1); // error
}

void SoftwareWire::i2c_bitbang()
{
  // Turn Arduino pin numbers into bit masks and io register addresses
  uint8_t sda_group = (_sdaPin&0xf0)>>4;
  uint8_t sda_pin = _sdaPin&0x0f;

  uint16_t *sda_idr = NULL;
  uint32_t *sda_cr = NULL;
  uint32_t sda_cr_mask;
  uint32_t sda_cr_in;
  uint32_t sda_cr_out;

  if (sda_pin < 0x08) {
    sda_cr = (uint32_t*)&gpio_group[sda_group]->CRL;
    sda_cr_mask = ~(0x0000000f << (sda_pin*4));
    sda_cr_in = (((uint32)GPI|(uint32)GPI_FLOATING_IN) << (sda_pin*4));
    sda_cr_out = (((uint32)GPO|(uint32)GPO_OPEN_DTAIN) << (sda_pin*4));
  } else {
    sda_cr = (uint32_t*)&gpio_group[sda_group]->CRH;
    sda_cr_mask = ~(0x0000000f << ((sda_pin-8)*4));
    sda_cr_in = (((uint32)GPI|(uint32)GPI_FLOATING_IN) << ((sda_pin-8)*4));
    sda_cr_out = (((uint32)GPO|(uint32)GPO_OPEN_DTAIN) << ((sda_pin-8)*4));
  }

  _idr_sda_mask = ((uint16_t)0x0001) << sda_pin;
  sda_idr     = (uint16_t*)&gpio_group[sda_group]->IDR;

  uint8_t scl_group = (_sclPin&0xf0)>>4;
  uint8_t scl_pin = _sclPin&0x0f;

  uint16_t *scl_idr = NULL;
  uint32_t *scl_cr = NULL;
  uint32_t scl_cr_mask;
  uint32_t scl_cr_in;
  uint32_t scl_cr_out;

  if (scl_pin < 0x08) {
    scl_cr = (uint32_t*)&gpio_group[scl_group]->CRL;
    scl_cr_mask = ~(0x0000000f << (scl_pin*4));
    scl_cr_in = (((uint32)GPI|(uint32)GPI_FLOATING_IN) << (scl_pin*4));
    scl_cr_out = (((uint32)GPO|(uint32)GPO_OPEN_DTAIN) << (scl_pin*4));
  } else {
    scl_cr = (uint32_t*)&gpio_group[scl_group]->CRH;
    scl_cr_mask = ~(0x0000000f << ((scl_pin-8)*4));
    scl_cr_in = (((uint32)GPI|(uint32)GPI_FLOATING_IN) << ((scl_pin-8)*4));
    scl_cr_out = (((uint32)GPO|(uint32)GPO_OPEN_DTAIN) << ((scl_pin-8)*4));
  }

  _idr_scl_mask = ((uint16_t)0x0001) << scl_pin;
  scl_idr     = (uint16_t*)&gpio_group[scl_group]->IDR;

  // i2c pins sda and scl have to be same io bank, same cr register
  if ((scl_cr != sda_cr) || (sda_idr != scl_idr)) i2c_bad_pins(); // error

  _i2c_idr = sda_idr;
  _i2c_cr = sda_cr;

  uint32_t cr_val = *_i2c_cr & sda_cr_mask & scl_cr_mask;

  _cr_sda_lo_scl_lo = cr_val | sda_cr_out | scl_cr_out;
  _cr_sda_lo_scl_hi = cr_val | sda_cr_out | scl_cr_in;
  _cr_sda_hi_scl_lo = cr_val | sda_cr_in  | scl_cr_out;
  _cr_sda_hi_scl_hi = cr_val | sda_cr_in  | scl_cr_in;
}

//
// Constructor
//
// The pins are not activated until begin() is called.
//
SoftwareWire::SoftwareWire()
{
}

SoftwareWire::SoftwareWire(uint8_t sdaPin, uint8_t sclPin, boolean pullups, boolean detectClockStretch)
{
  _sdaPin = sdaPin;
  _sclPin = sclPin;
  _pullups = pullups;
  _stretch = detectClockStretch;

  setClock( 40000UL);       // set default 40kHz

  // switch on gpio clocks, set pins to high (= input)
  gpio_init((PIN_enum)_sdaPin, GPI, 0, GPI_FLOATING_IN);
  gpio_init((PIN_enum)_sclPin, GPI, 0, GPI_FLOATING_IN);
  _sda_val = true;
  _scl_val = true;

  // Set default timeout to 1000 ms.
  // 1 second is very long, 10ms would be more appropriate.
  // However, the Arduino libraries use often a default timeout of 1 second.
  setTimeout( 20L);
}


//
// The destructor releases the pins Software I2C bus for other use.
//
SoftwareWire::~SoftwareWire()
{
  end();
}


//
// Release the pins of the Software I2C bus for other use.
// Also the internal pullup resistors are removed.
//
void SoftwareWire::end()
{
  // Remember the pullups variable.
  // They will be used again when begin() is called.
  boolean pullupsCopy = _pullups;

  _pullups = false;
  i2c_sda_hi();          // release sda, remove any pullup
  i2c_scl_hi();          // release scl, remove any pullup

  _pullups = pullupsCopy;
}


// begin(void) - enter master mode
// The pins are not changed until begin() is called.
void SoftwareWire::begin(void)
{
  rxBufPut = 0;          // nothing in the rxBuf
  rxBufGet = 0;

  i2c_init();            // release the sda and scl (the pullup resistors pull them high)

  // Some tests could be added here, to check if the SDA and SCL are really turning high.
  // Even some tests for shortcuts could be done here.


  // When a I2C transmission would start immediate, it could fail when only the internal pullup resistors
  // are used, and the signals were just now turned high with i2c_init().
  if( _pullups)
    delay(2);           // 1ms didn't always work.

  i2c_bitbang(); // calculate bit bang masks and registers
}

//
// beginTransmission starts the I2C transmission immediate.
//
void SoftwareWire::beginTransmission(uint8_t address)
{
  // Reset error returned by endTransmission.
  _transmission = SOFTWAREWIRE_NO_ERROR;

  // check return value of the start condition.
  // It indicates if the i2c bus is okay.
  if(i2c_start())
  {
    uint8_t rc = i2c_write((address << 1) | 0);       // The r/w bit is zero for write

    if( rc == 0)                                      // a sda zero from Slave for the 9th bit is ack
    {
      _transmission = SOFTWAREWIRE_NO_ERROR;
    }
    else
    {
      _transmission = SOFTWAREWIRE_ADDRESS_NACK;
    }
  }
  else
  {
    // If the bus was not okay, the scl or sda didn't work.
    _transmission = SOFTWAREWIRE_OTHER;
  }
}


//
void SoftwareWire::beginTransmission(int address)
{
  beginTransmission((uint8_t)address);
}


//
uint8_t SoftwareWire::endTransmission(boolean sendStop)
{
  if(sendStop)
    i2c_stop();
  else
    i2c_repstart();

  return(_transmission);          // return the transmission status that was set during writing address and data
}


//
// The requestFrom() read the data from the I2C bus and stores it in a buffer.
//
uint8_t SoftwareWire::requestFrom(uint8_t address, uint8_t size, boolean sendStop)
{
  uint8_t n=0;             // number of valid received bytes. Start with 0 bytes.

  // The transmission status is set, although it is not returned.
  // Start with the status : no error
  _transmission = SOFTWAREWIRE_NO_ERROR;


  // Clear the RX buffer
  rxBufPut = 0;
  rxBufGet = 0;

  boolean bus_okay = i2c_start();

  if(bus_okay)
  {
    uint8_t rc = i2c_write((address << 1) | 1);          // The r/w bit is '1' to read

    if( rc == 0)                                         // a sda zero from Slave for the 9th bit is ack
    {
      _transmission = SOFTWAREWIRE_NO_ERROR;

      // TODO: check if the Slave returns less bytes than requested.

      for(; n<size; n++)
      {
        if( n < (size - 1))
          rxBuf[n] = i2c_read(true);        // read with ack
        else
          rxBuf[n] = i2c_read(false);       // last byte, read with nack
      }
      rxBufPut = n;
    }
    else
    {
      _transmission = SOFTWAREWIRE_ADDRESS_NACK;
    }
  }
  else
  {
    // There was a bus error.
    _transmission = SOFTWAREWIRE_OTHER;
  }

  if(sendStop || _transmission != SOFTWAREWIRE_NO_ERROR)
    i2c_stop();
  else
    i2c_repstart();

  return( n);
}


//
uint8_t SoftwareWire::requestFrom(int address, int size, boolean sendStop)
{
  return requestFrom( (uint8_t) address, (uint8_t) size, sendStop);
}


// must be called in:
// slave tx event callback
// or after beginTransmission(address)
size_t SoftwareWire::write(uint8_t data)
{
  // When there was an error during the transmission, no more bytes are transmitted.
  if( _transmission == SOFTWAREWIRE_NO_ERROR)
  {
    if( i2c_write(data) == 0)                // a sda zero from Slave for the 9th bit is ack
    {
      _transmission = SOFTWAREWIRE_NO_ERROR;
    }
    else
    {
      _transmission = SOFTWAREWIRE_ADDRESS_NACK;
    }
  }

  return(1);             // ignore any errors, return the number of bytes that are written.
}


//
size_t SoftwareWire::write(const uint8_t* data, size_t quantity)
{
  for (size_t i=0; i<quantity; i++)
  {
    write(data[i]);
  }

  return(quantity);          // ignore any errors, return the number of bytes that are written.
}


//
int SoftwareWire::available(void)
{
  return(rxBufPut - rxBufGet);
}


//
int SoftwareWire::peek(void)
{
  int data;

  if( rxBufPut > rxBufGet)
  {
    data = rxBuf[rxBufGet];
  }
  else
  {
    data = -1;
  }

  return(data);
}


//
// The read() reads the buffer, not the I2C bus.
//
int SoftwareWire::read(void)
{
  int data;

  if( rxBufPut > rxBufGet)
  {
    data = rxBuf[rxBufGet];
    rxBufGet++;
  }
  else
  {
    data = -1;
  }

  return(data);
}


int SoftwareWire::readBytes(uint8_t* buf, uint8_t size)
{
  int data;
  int n;

  for( n=0; n<size; n++)
  {
    data = read();
    if( data == -1)
      break;
    else
      buf[n] = (uint8_t) data;
  }

  return(n);
}


//
int SoftwareWire::readBytes(char * buf, uint8_t size)
{
  return readBytes( (uint8_t *) buf, size);
}


//
int SoftwareWire::readBytes(char * buf, int size)
{
  return readBytes( (uint8_t *) buf, (uint8_t) size);
}


//
// Set the clock speed for the I2C bus.
// Default is 100000 (100kHz).
// A speed of 1Hz is possible with this software I2C library (but not with the Arduino Wire library).
// A speed of 200kHz or higher will remove the delay on an Arduino Uno.
// Without the delay, the functions are free running, using the execution timing of the code.
//
void SoftwareWire::setClock(uint32_t clock)
{
   if (clock >= 50000) _i2cdelay = 0;
   else _i2cdelay = 2;
}


//
// Set the timeout in milliseconds.
// At this moment, it is only used for timeout when the Slave is stretching the clock pulse.
//
void SoftwareWire::setTimeout(long timeout)
{
  // 2017, fix issue #6.
  // A signed long as parameter to be compatible with Arduino libraries.
  // A unsigned long internal to avoid compiler warnings.
  _timeout = (unsigned long) timeout;
}


// printStatus
// -----------
// Print information to the Serial port
// Used during developing and debugging.
// Call it with the Serial port as parameter:
//   myWire.printStatus(Serial);
// This function is not compatible with the Wire library.
// When this function is not called, it does not use any memory.
//
void SoftwareWire::printStatus( Print& Ser)
{
  Ser.println(F("-------------------"));
  Ser.println(F("SoftwareWire Status"));
  Ser.println(F("-------------------"));
  Ser.print(F("  F_CPU = "));
  Ser.println(SystemCoreClock);
  Ser.print(F("  sizeof(SoftwareWire) = "));
  Ser.println(sizeof(SoftwareWire));
  Ser.print(F("  _transmission status = "));
  Ser.println(_transmission);
  Ser.print(F("  _i2cdelay = "));
  Ser.print(_i2cdelay);
  if( _i2cdelay == 0)
    Ser.print(F(" (free running)"));
  Ser.println();
  Ser.print(F("  _pullups = "));
  Ser.print(_pullups);
  if( _pullups)
    Ser.print(F(" (enabled)"));
  Ser.println();
  Ser.print(F("  _timeout = "));
  Ser.print(_timeout);
  Ser.println(F(" ms"));

  Ser.print(F("  SOFTWAREWIRE_BUFSIZE = "));
  Ser.println(SOFTWAREWIRE_BUFSIZE);
  Ser.print(F("  rxBufPut = "));
  Ser.println(rxBufPut);
  Ser.print(F("  rxBufGet = "));
  Ser.println(rxBufGet);
  Ser.print(F("  available() = "));
  Ser.println(available());
  Ser.print(F("  rxBuf (hex) = "));
  for(int ii=0; ii<SOFTWAREWIRE_BUFSIZE; ii++)
  {
    if(rxBuf[ii] < 16)
      Ser.print(F("0"));
    Ser.print(rxBuf[ii],HEX);
    Ser.print(F(" "));
  }
  Ser.println();

  Ser.print(F("  _sdaPin = 0x"));
  Ser.println( (uint32_t) _sdaPin, HEX);
  Ser.print(F("  _sclPin = 0x"));
  Ser.println( (uint32_t) _sclPin, HEX);

  Ser.print(F("  _i2c_idr = 0x"));
  Ser.println( (uint32_t) _i2c_idr, HEX);
  Ser.print(F("  _idr_sda_mask = 0x"));
  Ser.println( (uint32_t) _idr_sda_mask, HEX);
  Ser.print(F("  _idr_scl_mask = 0x"));
  Ser.println( (uint32_t) _idr_scl_mask, HEX);

  Ser.print(F("  _i2c_cr = 0x"));
  Ser.println( (uint32_t) _i2c_cr, HEX);

  Ser.print(F("  _cr_sda_lo_scl_lo = 0x"));
  Ser.println( (uint32_t) _cr_sda_lo_scl_lo, HEX);
  Ser.print(F("  _cr_sda_lo_scl_hi = 0x"));
  Ser.println( (uint32_t) _cr_sda_lo_scl_hi, HEX);
  Ser.print(F("  _cr_sda_hi_scl_lo = 0x"));
  Ser.println( (uint32_t) _cr_sda_hi_scl_lo, HEX);
  Ser.print(F("  _cr_sda_hi_scl_hi = 0x"));
  Ser.println( (uint32_t) _cr_sda_hi_scl_hi, HEX);

  Ser.print(F("  line state sda = "));
  Ser.println(i2c_sda_read());
  Ser.print(F("  line state scl = "));
  Ser.println(i2c_scl_read());

#ifdef ENABLE_I2C_SCANNER
  // i2c_scanner
  // Taken from : http://playground.arduino.cc/Main/I2cScanner
  // At April 2015, it was version 5
  Ser.println("\n  I2C Scanner");
  byte error, address;
  int nDevices;

  Ser.println("  Scanning...");

  nDevices = 0;
  for(address=1; address<127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    beginTransmission(address);
    error = endTransmission();

    if (error == 0)
    {
      Ser.print("  I2C device found at address 0x");
      if (address<16)
        Ser.print("0");
      Ser.print(address,HEX);
      Ser.println("  !");

      nDevices++;
    }
    else if (error==4)
    {
      Ser.print("  Unknown error at address 0x");
      if (address<16)
        Ser.print("0");
      Ser.println(address,HEX);
    }
  }
  if (nDevices == 0)
    Ser.println("  No I2C devices found\n");
  else
    Ser.println("  done\n");
#endif
}


//--------------------------------------------------------------------


//
// The i2c_writebit and i2c_readbit could be make "inline", but that
// didn't increase the speed, and the code size increases.
//
// The sda is low after the start condition.
// Therefore the sda is low for the first bit.
//
void SoftwareWire::i2c_writebit(uint8_t c)
{
  if(c==0)
  {
    i2c_sda_lo();
  }
  else
  {
    i2c_sda_hi();
  }

  delayMicroseconds(1);             // i2c spec says "250ns data set-up time" (NXP UM10204, table 11)

  i2c_scl_hi();                     // clock high: the Slave will read the sda signal

  // Check if clock stretching by the Slave should be detected.
  if( _stretch)
  {
    // If the Slave was stretching the clock pulse, the clock would not go high immediately.
    // For example if the Slave is an Arduino, that has other interrupts running (for example Serial data).
    unsigned long prevMillis = millis();
    while( i2c_scl_read() == 0)
    {
      if( millis() - prevMillis >= _timeout)
        break;
    };
  }

  // After the clock stretching, the clock must be high for the normal duration.
  // That is why this delay has still to be done.
  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  i2c_scl_lo();

  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);
}


//
uint8_t SoftwareWire::i2c_readbit(void)
{
  i2c_sda_hi();            // 'hi' is the same as releasing the line
  i2c_scl_hi();

  // Check if clock stretching by the Slave should be detected.
  if( _stretch)
  {
    // Wait until the clock is high, the Slave could keep it low for clock stretching.
    unsigned long prevMillis = millis();
    while( i2c_scl_read() == 0)
    {
      if( millis() - prevMillis >= _timeout)
        break;
    };
  }

  // After the clock stretching, this delay has still be done before reading sda.
  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  uint8_t c = i2c_sda_read();

  i2c_scl_lo();

  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  return(c);
}


//
// Initializes the Software I2C.
//
// The original i2c_init sets the SDA and SCL high at the same time.
//
// The code has been changed, since the first data to the software i2c did fail sometimes.
// Changed into SCL high first, with a delay.
// That would send a STOP if the SDA happens to be low.
// Any Slave that was busy, will detect the STOP.
//
// After both lines are high, the delay is changed into 4 times the normal delay.
// That did reduce the error with the first transmission.
// It was tested with Arduino Uno with clock of 100kHz (_i2cdelay=2).
//
void SoftwareWire::i2c_init(void)
{
  i2c_scl_hi();

  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  i2c_sda_hi();

  for( uint8_t i=0; i<4; i++)             // 4 times the normal delay, to claim the bus.
  {
    if (_i2cdelay != 0)
      delayMicroseconds(_i2cdelay);
  }
}


//
// Send a START Condition
//
// The SDA and SCL should already be high.
//
// The SDA and SCL will both be low after this function.
// When writing the address, the Master makes them high.
//
// Return value:
//   true  : software i2c bus is okay.
//   false : failed, some kind of hardware bus error.
//
boolean SoftwareWire::i2c_start(void)
{
  i2c_sda_hi();              // can perhaps be removed some day ? if the rest of the code is okay
  i2c_scl_hi();              // can perhaps be removed some day ? if the rest of the code is okay

  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  // Both the sda and scl should be high.
  // If not, there might be a hardware problem with the i2c bus signal lines.
  // This check was added to prevent that a shortcut of sda would be seen as a valid ACK
  // from a i2c Slave.
  uint8_t sda_status = i2c_sda_read();
  uint8_t scl_status = i2c_scl_read();
  if(sda_status == 0 || scl_status == 0)
  {
    return(false);
  }
  else
  {
    i2c_sda_lo();

    if (_i2cdelay != 0)
      delayMicroseconds(_i2cdelay);

    i2c_scl_lo();

    if (_i2cdelay != 0)
      delayMicroseconds(_i2cdelay);
  }
  return(true);
}


//
// Repeated START instead of a STOP
//
// TODO: check if the repeated start actually works.
//
void SoftwareWire::i2c_repstart(void)
{
  i2c_sda_hi();
//  i2c_scl_hi();               // ??????

  i2c_scl_lo();                         // force SCL low

  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  i2c_sda_hi();                        // release SDA

  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  i2c_scl_hi();                        // release SCL

  // Check if clock stretching by the Slave should be detected.
  if( _stretch)
  {
    // If the Slave was stretching the clock pulse, the clock would not go high immediately.
    // For example if the Slave is an Arduino, that has other interrupts running (for example Serial data).
    unsigned long prevMillis = millis();
    while( i2c_scl_read() == 0)
    {
      if( millis() - prevMillis >= _timeout)
        break;
    };
  }

  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);
}


// Send a STOP Condition
//
// The stop was not recognized by every chip.
// Some code has been added (with comment "ADDED1"),
// to be sure that the levels are okay with delays in between.
void SoftwareWire::i2c_stop(void)
{
  i2c_scl_lo();         // ADDED1, it should already be low.
  i2c_sda_lo();

  // ADDED1, wait to be sure that the slave knows that both are low
  if (_i2cdelay != 0)              // ADDED1
    delayMicroseconds(_i2cdelay);  // ADDED1

  // For a stop, make SCL high wile SDA is still low
  i2c_scl_hi();

  // Check if clock stretching by the Slave should be detected.
  if( _stretch)
  {
    // Wait until the clock is high, the Slave could keep it low for clock stretching.
    // Clock pulse stretching during a stop condition seems odd, but when
    // the Slave is an Arduino, it might happen.
    unsigned long prevMillis = millis();
    while( i2c_scl_read() == 0)
    {
      if( millis() - prevMillis >= _timeout)
        break;
    };
  }

  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  // complete the STOP by setting SDA high
  i2c_sda_hi();

  // A delay after the STOP for safety.
  // It is not known how fast the next START will happen.
  if (_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);
}


//
// Write a byte to the I2C slave device
// The returned bit is 0 for ACK and 1 for NACK
//
uint8_t SoftwareWire::i2c_write( uint8_t c )
{
  for ( uint8_t i=0; i<8; i++)
  {
    i2c_writebit(c & 0x80);           // highest bit first
    c <<= 1;
  }

  return(i2c_readbit());
}


//
// read a byte from the I2C slave device
//
uint8_t SoftwareWire::i2c_read(boolean ack)
{
  uint8_t res = 0;

  for(uint8_t i=0; i<8; i++)
  {
    res <<= 1;
    res |= i2c_readbit();
  }

  if(ack)
  {
    i2c_writebit(0);
  }
  else
  {
    i2c_writebit(1);
  }

  if(_i2cdelay != 0)
    delayMicroseconds(_i2cdelay);

  return(res);
}
