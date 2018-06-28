//-----------------------------------------------------------------------------
//  File          : libc.c                                                     
//  Module        :                                                            
//  Description   : Include Function to replace in Libc.
//  Author        : 
//  Notes         :                                                            
//                                                                             
// ============================================================================
// | Naming conventions                                                       |
// | ~~~~~~~~~~~~~~~~~~                                                       |
// |         Struct/Array : Leading s                                         |
// |             Constant : Leading K                                         |
// |      Global Variable : Leading g  (variable use outside one C file)      |
// |                                                                          |
// |   BYTE( 8 Bits) size : Leading b                                         |
// |   CHAR( 8 Bits) size : Leading c                                         |
// |   WORD(16 Bits) size : Leading w                                         |
// |  DWORD(32 Bits) size : Leading d                                         |
// |    int(32 Bits) size : Leading i                                         |
// | DDWORD(64 Bits) size : Leading dd                                        |
// |              Pointer : Leading p                                         |
// |                                                                          |
// |    Function argument : Leading a                                         |
// |       Local Variable : All lower case                                    |
// |                                                                          |
// | Examples:                                                                |
// |                Array : Leading s, (sb = arrary of byte, etc)             |
// |              Pointer : Leading p, (pb = pointer of byte, etc)            |
// ============================================================================
//                                                                             
//  Date          Who         Action                                           
//  ------------  ----------- -------------------------------------------------
//  DD MMM  YYYY  main_c      From previous syscalls.c
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdarg.h>
#include <reent.h>
#include <sys/stat.h>
#include "system.h"


//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define PAD_RIGHT 1
#define PAD_ZERO  2
#define STR_LIMIT 4
/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

_ssize_t _read_r(struct _reent *r, int file, void *ptr, size_t len) 
{
  int i, ch;
  unsigned char *p;
  
  p = (unsigned char*)ptr;
  
  for (i=0; i<len; i++) {
    ch = os_kbd_getkey();
    if (ch == 0) {
      return(i);
    }
    p[i] = ch;
    os_sleep ();
  }
  
  return(i);
}

_ssize_t _write_r (struct _reent *r, int file, const void *ptr, size_t len)
{
  int todo; 
  const unsigned char *p;

  p = (const unsigned char*) ptr;

  if (ptr == 0) {  // flush internal buffers
    return 0;
  }

  for (todo = 0; todo < len; todo++) { 
    os_disp_putc(*p++);    // put to display
  } 
  return len; 
}

int _close_r(struct _reent *r, int file)
{
  return 0;
}

_off_t _lseek_r(struct _reent *r, int file, _off_t ptr, int dir)
{
  return (_off_t)0;  /*  Always indicate we are at file beginning.  */
}

int _fstat_r(struct _reent *r, int file, struct stat *st)
{
  /*  Always set as character device.        */
  st->st_mode = S_IFCHR;

  /* assigned to strong type with implicit   */
  /* signed/unsigned conversion.  Required by   */
  /* newlib.          */
  return 0;
}

int isatty(int file)
{
  return 1;
}

extern char _end[];             /*  end is set in the linker command   */
extern char _end_of_bss[];      // PC0711 value from linker
                                /* file and is the end of statically   */
                                /* allocated data (thus start of heap).  */

static char *heap_ptr;          /* Points to current end of the heap.  */

/************************** _sbrk_r *************************************
 * Support function. Adjusts end of heap to provide more memory to
 * memory allocator. Simple and dumb with no sanity checks.
 *  struct _reent *r -- re-entrancy structure, used by newlib to
 *                      support multiple threads of operation.
 *  ptrdiff_t nbytes -- number of bytes to add.
 *                      Returns pointer to start of new heap area.
 *
 *  Note:  This implementation is not thread safe (despite taking a
 *         _reent structure as a parameter).
 *         Since _s_r is not used in the current implementation, 
 *         the following messages must be suppressed.
 */
void * _sbrk_r(struct _reent *_s_r, ptrdiff_t nbytes)
{
  char  *base;    /*  errno should be set to  ENOMEM on error  */

  if (!heap_ptr) {  /*  Initialize if first time through.  */
    heap_ptr = _end;
  }
  base = heap_ptr;  /*  Point to end of heap.  */
  heap_ptr += nbytes;  /*  Increase heap.  */

  //check if overfall
  if (heap_ptr >= _end_of_bss) {
    return (caddr_t) -1;
  }
  
  return base;    /*  Return pointer to start of new heap area.  */
}

//PC1602 override newlib
int putchar(int c)
{
  os_disp_putc(c);
  return 1;
}

static void printchar(unsigned char **str, int c)
{
  if (str) {
    **str = c;
    ++(*str);
  }
  else 
    os_disp_putc(c);
}

static int prints(unsigned char **out, const unsigned char *string, int width, int pad)
{
  register int pc = 0, padchar = ' ';

  if (!(pad & STR_LIMIT)) {
    // normal string output
    if (width > 0) {
      register int len = 0;
      register const unsigned char *ptr;
      for (ptr = string; *ptr; ++ptr) ++len;
      if (len >= width) 
        width = 0;
      else 
        width -= len;
      if (pad & PAD_ZERO) 
        padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
      for ( ; width > 0; --width) {
        printchar (out, padchar);
        ++pc;
      }
    }
    for ( ; *string ; ++string) {
      printchar (out, *string);
      ++pc;
    }
    for ( ; width > 0; --width) {
      printchar (out, padchar);
      ++pc;
    }
  }
  else {
    // limited string output
    for ( ; *string && (width>0); ++string, --width) {
      printchar (out, *string);
      ++pc;
    }
  }

  return pc;
}

static int printi(unsigned char **out, int i, int b, int sg, int width, int pad, int letbase)
{
  unsigned char print_buf[PRINT_BUF_LEN];
  register unsigned char *s;
  register int t, neg = 0, pc = 0;
  register unsigned int u = i;
  
  if (i == 0) {
    print_buf[0] = '0';
    print_buf[1] = '\0';
    return prints (out, print_buf, width, pad);
  }
  
  if (sg && b == 10 && i < 0) {
    neg = 1;
    u = -i;
  }
  
  s = print_buf + PRINT_BUF_LEN-1;
  *s = '\0';
  
  while (u) {
    t = u % b;
    if ( t >= 10 )
      t += letbase - '0' - 10;
    *--s = t + '0';
    u /= b;
  }
  
  if (neg) {
    if ( width && (pad & PAD_ZERO) ) {
      printchar (out, '-');
      ++pc;
      --width;
    }
    else {
      *--s = '-';
    }
  }
  
  return pc + prints (out, s, width, pad);
}

static int printi_f(unsigned char **out, double f, int decpt)
{
  unsigned char print_buf[PRINT_BUF_LEN];
  register unsigned char *s;
  register int t=0, neg=0, pad, dot=0;
  long long u, exp=1;
  //float ff = (float)f;
  double ff = f;
  register unsigned char *pf;

  if (ff == 0) {
    print_buf[t++] = '0';
    if (decpt)
      print_buf[t++] = '.';
    print_buf[t] = '\0';
    pad = PAD_RIGHT | PAD_ZERO;
    return prints (out, print_buf, decpt+t, pad);
  }

  // clear the signed bit in the floating point no. (little endian)
  pf = (unsigned char *)&ff;
  if (sizeof(double) == 8) {
    // for GCC compiler
    if (pf[7] & 0x80) {
      neg = 1;
      pf[7] &= 0x7F;
    }
  }
  else if (sizeof(double) == 4) {
    // for Yagarto compiler
    if (pf[3] & 0x80) {
      neg = 1;
      pf[3] &= 0x7F;
    }
  }

  s = print_buf + PRINT_BUF_LEN-1;
  *s = '\0';

  t = decpt;
  while (t--)
    exp *= 10;
  ff += (0.1 / exp);    // output tuning
  u = ff * exp;         // convert to normal long long

  while (1) {
    // exit if value=0 AND '.' is printed or decpt=0
    if (!u && ((decpt && dot) || !decpt))
      break;
    t = u % 10;
    *--s = t + '0';
    // display '.' for decpt > 0 only
    if((print_buf+PRINT_BUF_LEN-1-s) == decpt) {
      *--s = '.';
      dot = 1;
      if ((u / 10) == 0) {
        *--s = '0';
        break;
      }
    }
    u /= 10;
  }

  if (neg)
    *--s = '-';

  return prints (out, s, 0, 0);
}

static int print(unsigned char **out, const unsigned char *format, va_list args )
{
  register int width, pad, decpt;
  register int pc = 0;
  unsigned char scr[2];

  for (; *format != 0; ++format) {
    if (*format == '%') {
      ++format;
      width = pad = 0;
      decpt = 2;    // default decimal point is 2
      if (*format == '\0') break;
      if (*format == '%') goto out;
      if (*format == '-') {
        ++format;
        pad = PAD_RIGHT;
      }
      if (*format == '.') {
        ++format;
        if (*(format+1) == 'f') {
          decpt = *format - '0';
          if (decpt > 7)
            decpt = 7;
          ++format;
        }
        if (*(format+1) == 's')
          pad |= STR_LIMIT;
      }
      while (*format == '0') {
        ++format;
        pad |= PAD_ZERO;
     }
      for ( ; *format >= '0' && *format <= '9'; ++format) {
        width *= 10;
        width += *format - '0';
      }
      if ( *format == 's' ) {
        register char *s = (char *)va_arg( args, int );
        pc += prints (out, s?s:"(null)", width, pad);
        continue;
      }
      if (( *format == 'd' )||( *format == 'i' )) {
        pc += printi (out, va_arg( args, int ), 10, 1, width, pad, 'a');
        continue;
      }
      if ( *format == 'x' ) {
        pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'a');
        continue;
      }
      if ( *format == 'X' ) {
        pc += printi (out, va_arg( args, int ), 16, 0, width, pad, 'A');
        continue;
      }
      if ( *format == 'u' ) {
        pc += printi (out, va_arg( args, int ), 10, 0, width, pad, 'a');
        continue;
      }
      if ( *format == 'c' ) {
        /* char are converted to int then pushed on the stack */
        scr[0] = (unsigned char)va_arg( args, int );
        scr[1] = '\0';
        pc += prints (out, scr, width, pad);
        continue;
      }
      if( *format == 'f' ) {
        pc += printi_f (out, va_arg( args, double ), decpt);
        continue;
      }
    }
    else {
      out:
      printchar (out, *format);
      ++pc;
    }
  }
  if (out) 
    **out = '\0';
  va_end( args );
  return pc;
}

int printf(const unsigned char *format, ...)
{
  va_list args;
  va_start( args, format );
  return print( 0, format, args );
}

int sprintf(unsigned char *out, const unsigned char *format, ...)
{
  va_list args;
        
  va_start( args, format );
  return print( &out, format, args );
}

