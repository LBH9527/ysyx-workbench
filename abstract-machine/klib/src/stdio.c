#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define PRINTF_BUF_LENGTH  128
char pBuf[PRINTF_BUF_LENGTH];

int printf(const char *fmt, ...) 
{
  // panic("Not implemented");
  int ret = 0;
  

  va_list ap;
  va_start(ap, fmt);
  ret = vsprintf(pBuf, fmt, ap);
  va_end(ap);

  putstr(pBuf);

  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) 
{
    return (vsnprintf(out, PRINTF_BUF_LENGTH, fmt, ap));
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
#if 1
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
#endif



//------------------------------------------------------------------------------
// Writes a character inside the given string. Returns 1.
// \param pStr  Storage string.
// \param c  Character to write.
//------------------------------------------------------------------------------
signed int PutChar(char *pStr, char c)
{
    *pStr = c;
    return 1;
}

//------------------------------------------------------------------------------
// Writes a string inside the given string.
// Returns the size of the written
// string.
// \param pStr  Storage string.
// \param pSource  Source string.
//------------------------------------------------------------------------------
signed int PutString(char *pStr, const char *pSource)
{
    signed int  num = 0;
    
    while(*pSource != 0)
    {
      *pStr = *pSource;
      pStr++;
      pSource++;
      num ++;
    }

    return num;
}

//------------------------------------------------------------------------------
// Writes an unsigned int inside the given string, using the provided fill &
// width parameters.
// Returns the size in characters of the written integer.
// \param pStr  Storage string.
// \param fill  Fill character.
// \param width  Minimum integer width.
// \param value  Integer value.
//------------------------------------------------------------------------------
signed int PutUnsignedInt(
    char *pStr,
    char fill,
    signed int width,
    unsigned int value)
{
    signed int num = 0;

    // Take current digit into account when calculating width
    width--;

    // Recursively write upper digits
    if ((value / 10) > 0) {

        num = PutUnsignedInt(pStr, fill, width, value / 10);
        pStr += num;
    }
    // Write filler characters
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    // Write lower digit
    num += PutChar(pStr, (value % 10) + '0');

    return num;
}
// #TODO  用非递归（recursion ）的方式来实现。
//------------------------------------------------------------------------------
// Writes a signed int inside the given string, using the provided fill & width
// parameters.
// Returns the size of the written integer.
// \param pStr  Storage string.
// \param fill  Fill character.
// \param width  Minimum integer width.
// \param value  Signed integer value.
//------------------------------------------------------------------------------
#if 0
signed int PutSignedInt(
    char *pStr,
    char fill,
    signed int width,
    signed int value)
{
    signed int num = 0;
    unsigned int absolute;

    // Compute absolute value
    if (value < 0) {

        absolute = -value;
    }
    else {

        absolute = value;
    }

    // Take current digit into account when calculating width
    width--;

    // Recursively write upper digits
    if ((absolute / 10) > 0) {

        if (value < 0) {
        
            num = PutSignedInt(pStr, fill, width, -(absolute / 10));
        }
        else {

            num = PutSignedInt(pStr, fill, width, absolute / 10);
        }
        pStr += num;
    }
    else {

        // Reserve space for sign
        if (value < 0) {

            width--;
        }

        // Write filler characters
        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }

        // Write sign
        if (value < 0) {

            num += PutChar(pStr, '-');
            pStr++;
        }
    }

    // Write lower digit
    num += PutChar(pStr, (absolute % 10) + '0');

    return num;
}
#else
signed int PutSignedInt(
    char *pStr,
    char fill,
    signed int width,
    signed int value)
{
    uint32_t i;
    uint32_t length ;
    signed int  num = 0;
    char *putstr;

    num = width; 
    putstr = pStr;
    while(width)
    {
      *putstr = fill;
      putstr++;
      width--;
    }

    i = 1;
    length = 0;
    do
    {
      length++;
      i = i*10;
    }while(value / i );
    
    if (length > num )
    {
      num = length;
    }
    else
    {
      length =  num;
    }
    
    putstr = pStr;
    while(length)
    {
      length--;
      *(putstr + length)  = (char)(value % 10 + 0x30);   //0x30 : digital to ascii
      value = value / 10;
    }

    return num;
}
#endif
//------------------------------------------------------------------------------
// Writes an hexadecimal value into a string, using the given fill, width &
// capital parameters.
// Returns the number of char written.
// \param pStr  Storage string.
// \param fill  Fill character.
// \param width  Minimum integer width.
// \param maj  Indicates if the letters must be printed in lower- or upper-case.
// \param value  Hexadecimal value.
//------------------------------------------------------------------------------
signed int PutHexa(
    char *pStr,
    char fill,
    signed int width,
    unsigned char maj,
    unsigned int value)
{
    signed int num = 0;

    // Decrement width
    width--;

    // Recursively output upper digits
    if ((value >> 4) > 0) {

        num += PutHexa(pStr, fill, width, maj, value >> 4);
        pStr += num;
    }
    // Write filler chars
    else {

        while (width > 0) {

            PutChar(pStr, fill);
            pStr++;
            num++;
            width--;
        }
    }

    // Write current digit
    if ((value & 0xF) < 10) {

        PutChar(pStr, (value & 0xF) + '0');
    }
    else if (maj) {

        PutChar(pStr, (value & 0xF) - 10 + 'A');
    }
    else {

        PutChar(pStr, (value & 0xF) - 10 + 'a');
    }
    num++;

    return num;
}

//*****************************************************************************
//
//  Float to ASCII text. A basic implementation for providing support for
//  single-precision %f.
//
//  param
//      fValue     = Float value to be converted.
//      pcBuf      = Buffer to place string AND input of buffer size.
//      iPrecision = Desired number of decimal places.
//      IMPORTANT: On entry, the first 32-bit word of pcBuf must
//                 contain the size (in bytes) of the buffer!
//                 The recommended size is at least 16 bytes.
//
//  This function performs a basic translation of a floating point single
//  precision value to a string.
//
//  return Number of chars printed to the buffer.
//
//*****************************************************************************
#define AM_FTOA_ERR_VAL_TOO_SMALL   -1
#define AM_FTOA_ERR_VAL_TOO_LARGE   -2
#define AM_FTOA_ERR_BUFSIZE         -3

typedef union
{
    int32_t I32;
    float F;
} i32fl_t;

static int Putfloat( char *pcBuf, int iPrecision, float fValue)
{
    i32fl_t unFloatValue;
    int iExp2, iBufSize;
    int32_t i32Significand, i32IntPart, i32FracPart;
    char *pcBufInitial, *pcBuftmp;

    iBufSize = *(uint32_t*)pcBuf;
    if (iBufSize < 4)
    {
        return AM_FTOA_ERR_BUFSIZE;
    }

    if (fValue == 0.0f)
    {
        // "0.0"
        *(uint32_t*)pcBuf = 0x00 << 24 | ('0' << 16) | ('.' << 8) | ('0' << 0);
        return 3;
    }

    pcBufInitial = pcBuf;

    unFloatValue.F = fValue;

    iExp2 = ((unFloatValue.I32 >> 23) & 0x000000FF) - 127;
    i32Significand = (unFloatValue.I32 & 0x00FFFFFF) | 0x00800000;
    i32FracPart = 0;
    i32IntPart = 0;

    if (iExp2 >= 31)
    {
        return AM_FTOA_ERR_VAL_TOO_LARGE;
    }
    else if (iExp2 < -23)
    {
        return AM_FTOA_ERR_VAL_TOO_SMALL;
    }
    else if (iExp2 >= 23)
    {
        i32IntPart = i32Significand << (iExp2 - 23);
    }
    else if (iExp2 >= 0)
    {
        i32IntPart = i32Significand >> (23 - iExp2);
        i32FracPart = (i32Significand << (iExp2 + 1)) & 0x00FFFFFF;
    }
    else // if (iExp2 < 0)
    {
        i32FracPart = (i32Significand & 0x00FFFFFF) >> -(iExp2 + 1);
    }

    if (unFloatValue.I32 < 0)
    {
        *pcBuf++ = '-';
    }

    if (i32IntPart == 0)
    {
        *pcBuf++ = '0';
    }
    else
    {
        if (i32IntPart > 0)
        {
            uint64_to_str(i32IntPart, pcBuf);
        }
        else
        {
            *pcBuf++ = '-';
            uint64_to_str(-i32IntPart, pcBuf);
        }
        while (*pcBuf)    // Get to end of new string
        {
            pcBuf++;
        }
    }

    //
    // Now, begin the fractional part
    //
    *pcBuf++ = '.';

    if (i32FracPart == 0)
    {
        *pcBuf++ = '0';
    }
    else
    {
        int jx, iMax;

        iMax = iBufSize - (pcBuf - pcBufInitial) - 1;
        iMax = (iMax > iPrecision) ? iPrecision : iMax;

        for (jx = 0; jx < iMax; jx++)
        {
            i32FracPart *= 10;
            *pcBuf++ = (i32FracPart >> 24) + '0';
            i32FracPart &= 0x00FFFFFF;
        }

        //
        // Per the printf spec, the number of digits printed to the right of the
        // decimal point (i.e. iPrecision) should be rounded.
        // Some examples:
        // Value        iPrecision          Formatted value
        // 1.36399      Unspecified (6)     1.363990
        // 1.36399      3                   1.364
        // 1.36399      4                   1.3640
        // 1.36399      5                   1.36399
        // 1.363994     Unspecified (6)     1.363994
        // 1.363994     3                   1.364
        // 1.363994     4                   1.3640
        // 1.363994     5                   1.36399
        // 1.363995     Unspecified (6)     1.363995
        // 1.363995     3                   1.364
        // 1.363995     4                   1.3640
        // 1.363995     5                   1.36400
        // 1.996        Unspecified (6)     1.996000
        // 1.996        2                   2.00
        // 1.996        3                   1.996
        // 1.996        4                   1.9960
        //
        // To determine whether to round up, we'll look at what the next
        // decimal value would have been.
        //
        if ( ((i32FracPart * 10) >> 24) >= 5 )
        {
            //
            // Yes, we need to round up.
            // Go back through the string and make adjustments as necessary.
            //
            pcBuftmp = pcBuf - 1;
            while ( pcBuftmp >= pcBufInitial )
            {
                if ( *pcBuftmp == '.' )
                {
                }
                else if ( *pcBuftmp == '9' )
                {
                    *pcBuftmp = '0';
                }
                else
                {
                    *pcBuftmp += 1;
                    break;
                }
                pcBuftmp--;
            }
        }
    }

    //
    // Terminate the string and we're done
    //
    *pcBuf = 0x00;

    return (pcBuf - pcBufInitial);
} // ftoa()


//******************************************************************************
//
/// Stores the result of a formatted string into another string. Format
/// arguments are given in a va_list instance.
/// Return the number of characters written.
/// \param out    Destination string.
/// \param length  Length of Destination string.
/// \param pFormat Format string.
/// \param ap      Argument list.
//!
//! A lite version of vsprintf().
//!      Currently handles the following specifiers:
//!      %c
//!      %s
//!      %[0][width]d (also %i)
//!      %[0][width]u
//!      %[0][width]x
//!      %[.precision]f
//!
//!     Note than any unrecognized or unhandled format specifier character is
//!     simply printed.  For example, "%%" will print a '%' character.
//!
//! @return uint32_t representing the number of characters printed.
//
//******************************************************************************
int vsnprintf(char *out, size_t length, const char *fmt, va_list ap) {
  const char *pFormat;
  char *pDst;
  char          fill;
  unsigned char width;
  signed int    num = 0;
  int char_count = 0;
  int iPrecision;

  pFormat = fmt;
  pDst = out;

  while (*pFormat != 0 && char_count < length)
  {
    iPrecision = 6;             // printf() default precision for %f is 6
    // Normal character
    if(*pFormat != '%')
    {
        *pDst = *pFormat;
        pDst++;
        pFormat++;
        char_count ++;
    }
    // Escaped '%' (writes literal %. The full conversion specification must be %%.)
    else if(*(pFormat+1) == '%' )
    {
        *pDst = '%';
        pDst ++;
        pFormat += 2;
        char_count ++;
    }
    // Token delimiter
    else
    {
      fill = ' ';
      width = 0;
      pFormat++;

      // Parse filler
      if(*pFormat == '0')
      {
        fill = '0';
        pFormat++;
      }
      // Parse width
      while ( (*pFormat >= '0') && (*pFormat <= '9') )
      {
          width = width * 10 + (*pFormat - '0');
          pFormat ++;
      }
      // Check if there is enough space
      if (char_count + width > length) 
      {
          width = length - char_count;
      }

      switch (*pFormat) 
      {
        case 'd': 
        case 'i': num = PutSignedInt(pDst, fill, width, va_arg(ap, signed int)); break;
        case 'u': num = PutUnsignedInt(pDst, fill, width, va_arg(ap, unsigned int)); break;
        // The void * pointer argument is printed in hexadecimal (as if by %#x or %#lx).
        case 'p': num = PutHexa(pDst, fill, width, 0, va_arg(ap, unsigned int)); break;
        case 'x': num = PutHexa(pDst, fill, width, 0, va_arg(ap, unsigned int)); break;
        case 'X': num = PutHexa(pDst, fill, width, 1, va_arg(ap, unsigned int)); break;
        case 's': num = PutString(pDst, va_arg(ap, char *)); break;
        case 'c': num = PutChar(pDst, va_arg(ap, unsigned int)); break;
        case 'f': 
        {
          num = Putfloat(pDst, iPrecision, va_arg(ap, double)); 
          if ( num < 0 )
          {
              uint32_t u32PrntErrVal;
              if ( num == AM_FTOA_ERR_VAL_TOO_SMALL )
              {
                  u32PrntErrVal = (0x00 << 24) | ('0' << 16) |
                                  ('.' << 8)   | ('0' << 0);  // "0.0"
              }
              else if ( num == AM_FTOA_ERR_VAL_TOO_LARGE )
              {
                  u32PrntErrVal = (0x00 << 24) | ('#' << 16) |
                                  ('.' << 8)   | ('#' << 0);  // "#.#"
              }
              else
              {
                  u32PrntErrVal = (0x00 << 24) | ('?' << 16) |
                                  ('.' << 8)   | ('?' << 0);  // "?.?"
              }
              *(uint32_t*)pDst = u32PrntErrVal;
              num = 3;
          }
          
          break;
        }
        default:
            assert(0);
            return -1;
      }
      pFormat++;
      pDst += num;
      char_count += num;
    }

  }

   // NULL-terminated (final \0 is not counted)
  if (char_count < length)
  {
      *pDst = 0;
  }
  else
  {
      *(--pDst) = 0;
      char_count --;
  }
   
  return char_count;
}

#endif
