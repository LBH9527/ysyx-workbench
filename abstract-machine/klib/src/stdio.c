#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define PRINTF_BUF_LENGTH  128

int printf(const char *fmt, ...) 
{
  // panic("Not implemented");
  int ret = 0;
  char pBuf[PRINTF_BUF_LENGTH];

  va_list ap;
  va_start(ap, fmt);
  ret = vsprintf(pBuf, fmt, ap);
  va_end(ap);

  putstr(pBuf);

  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) 
{
    return (vsnprintf(out, sizeof(out), fmt, ap));
}

int sprintf(char *out, const char *fmt, ...) {
  // panic("Not implemented");
  int ret = 0;
   va_list ap;

  va_start(ap, fmt);
  ret = vsprintf(out, fmt, ap);
  va_end(ap);

  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  // panic("Not implemented");
  int ret = 0;
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(out, n, fmt, ap);
  va_end(ap);

  return ret;
}

static int uint64_to_str(uint64_t ui64Val, char *pcBuf)
{
  uint32_t i;
  uint32_t length ;
  char *psrc;
  int ret = 0;
 
  psrc = pcBuf;
  i = 1;
  length = 0;
  while(ui64Val / i )
  {
    length++;
    i = i*10;
  }
  ret = length;

  while(length)
  {
    length--;
    *(psrc + length)  = (char)(ui64Val % 10 + 0x30);   //0x30 : digital to ascii
    ui64Val = ui64Val / 10;    
  }

  return ret;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  // panic("Not implemented");
  // uint32_t i;
  // uint32_t flag;
  const char *src;
  char *dst;
  int d;
  char *s;
  // char c;
  int temp;
  int char_count = 0;

  src = fmt;
  dst = out;

  while (*src)
  {
    if (*src == '%')
    { 
      src++;
      switch (*src) 
      {
        case 's':              /* string */
            s = va_arg(ap, char *);
            while(*s != '\0')
            {
              *dst = *s;
              dst ++;
              s ++;
              char_count++;
            }  
            break;
        case 'd':              /* int */
            d = va_arg(ap, int);
            temp = uint64_to_str(d, dst);
            dst += temp;           
            char_count += temp; 
            break;
        // case 'c':              /* char */
        //     /* need a cast here since va_arg only
        //       takes fully promoted types */
        //     c = (char) va_arg(ap, int);
        //     break;
        default:
            assert(0);
            break;
      }
      src++;
    }
    else
    {
      *dst = *src;
      src++;
      dst++;
      char_count++;
    }
  }

  //
  // Terminate the string
  //
  if ( dst )
  {
      *dst = '\0';
  }

  return char_count;
}

#endif
