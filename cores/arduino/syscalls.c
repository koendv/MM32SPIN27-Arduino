/**
  * \file syscalls_stm32.c
  *
  * Implementation of newlib syscall.
  *
  */

#if defined (  __GNUC__  ) /* GCC CS3 */
  #include <sys/stat.h>
#endif
#include <errno.h>
#undef errno
extern int errno;

extern size_t uart_debug_write(uint8_t *data, uint32_t size);

// Helper macro to mark unused parameters and prevent compiler warnings.
// Appends _UNUSED to the variable name to prevent accidentally using them.
#ifdef UNUSED
  #undef UNUSED
#endif
#ifdef __GNUC__
  #define UNUSED(x) x ## _UNUSED __attribute__((__unused__))
#else
  #define UNUSED(x) x ## _UNUSED
#endif

#ifdef __GNUC__
/* from cmsis_gcc.h */

/**
  \brief   Get Main Stack Pointer
  \details Returns the current value of the Main Stack Pointer (MSP).
  \return               MSP Register value
 */
__attribute__((always_inline)) static inline uint32_t __get_MSP(void)
{
    register uint32_t result;

    __asm volatile ("MRS %0, msp" : "=r" (result) );
    return(result);
}
#endif

__attribute__((weak))
caddr_t _sbrk(int incr)
{
  extern char _estack; /* Defined in the linker script */
  extern char _Min_Stack_Size; /* Defined in the linker script */
  extern char _end; /* Defined by the linker */
  static char *heap_end = &_end ;
  char *prev_heap_end = heap_end;

  if (heap_end + incr > (char *)__get_MSP()) {
    /* Heap and stack collision */
    errno = ENOMEM;
    return (caddr_t) -1;
  }

  /* Ensure to keep minimum stack size defined in the linker script */
  if (heap_end + incr >= (char *)(&_estack - &_Min_Stack_Size)) {
    errno = ENOMEM;
    return (caddr_t) -1;
  }

  heap_end += incr ;
  return (caddr_t) prev_heap_end ;
}

__attribute__((weak))
int _close(UNUSED(int file))
{
  return -1;
}

__attribute__((weak))
int _fstat(UNUSED(int file), struct stat *st)
{
  st->st_mode = S_IFCHR ;
  return 0;
}

__attribute__((weak))
int _isatty(UNUSED(int file))
{
  return 1;
}

__attribute__((weak))
int _lseek(UNUSED(int file), UNUSED(int ptr), UNUSED(int dir))
{
  return 0;
}

__attribute__((weak))
int _read(UNUSED(int file), UNUSED(char *ptr), UNUSED(int len))
{
  return 0;
}

/* Moved to Print.cpp to support Print::printf()
   If you get a linker error "undefined reference to _write",
   add "#include <Print.h>" to your Arduino sketch.
*/
__attribute__((weak))
int _write(UNUSED(int file), char *ptr, int len)
{
}

__attribute__((weak))
void _exit(UNUSED(int status))
{
  for (; ;) ;
}

__attribute__((weak))
int _kill(UNUSED(int pid), UNUSED(int sig))
{
  errno = EINVAL;
  return -1;
}

__attribute__((weak))
int _getpid(void)
{
  return 1;
}
