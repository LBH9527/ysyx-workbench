#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// strlen is with out null character
size_t strlen(const char *s) {
  // panic("Not implemented");
  assert(s != NULL);
  const char *ps = s;
  size_t len = 0;

  while(*ps != '\0')
  {
    ps++;
    len++;
  }

  return len;
}

char *strcpy(char *dst, const char *src) {
  // panic("Not implemented");
  assert(dst != NULL);
  assert(src != NULL);
  assert(sizeof(dst) >= sizeof(src));

  char *pdst = dst;
  const char *psrc = src;

  while(*psrc != '\0')
  {
    *pdst = *psrc;
    pdst++;
    psrc++;
  }

  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  // panic("Not implemented");
  assert(dst != NULL);
  assert(src != NULL);
  assert(sizeof(dst) >= n);

  char *pdst = dst;
  const char *psrc = src;
  while(n--)
  {
    *pdst = *psrc;
    pdst++;
    psrc++;
  }

  return dst;
}

char *strcat(char *dst, const char *src) {
  // panic("Not implemented");
  assert(dst != NULL);
  assert(src != NULL);
  assert(sizeof(dst) > (strlen(dst) + strlen(src)) );
  char *pdst = dst;
  const char *psrc = src;
 
  while(*pdst != '\0')
  {
    pdst++;
  }

  while(*psrc != '\0')
  {
    *pdst = *psrc;  
    pdst++;
    psrc++;
  }   

  return dst;
}

//The  strcmp()  function compares the two strings s1 and s2.  
// The locale is not taken into account (for a locale-aware comparison, see strcoll(3)).  
// It returns an integer less than, equal to, or greater than zero if s1 is found, respectively, 
// to be less than, to match, or be greater than s2.
int strcmp(const char *s1, const char *s2) {
  // panic("Not implemented");
  assert(s1 != NULL && s2 != NULL);
  const char *ps1 = s1;
  const char *ps2 = s2;

  while(*ps1 != '\0')
  {
    if(*ps1 == *ps2)
    {
      ps1++;
      ps2++;
    }
    else
    {
      break;
    }
  }
  
  return (*ps1 - *ps2);

}

int strncmp(const char *s1, const char *s2, size_t n) {
  // panic("Not implemented");
  assert(s1 != NULL && s2 != NULL);
  const char *ps1 = s1;
  const char *ps2 = s2;

  while(n--)
  {
    if(*ps1 == *ps2)
    {
      ps1++;
      ps2++;
    }
    else
    {
      break;
    }
  }
  
  return (*ps1 - *ps2);
}

void *memset(void *s, int c, size_t n) {
  // panic("Not implemented");
  assert(s != NULL);
  // printf("sizeof s is %d \r\n", sizeof(s));
  assert(sizeof(s) >= n) ;
  void *ps = s;

  while(n--)
  {
    *(int*)ps = c;
    ps++;
  }

  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  // panic("Not implemented");
  assert(dst != NULL);
  assert(src != NULL);

  char *p_dst = dst;
  const char* p_src = src;
  size_t i;

  for(i=0; i<n; i++)
  {
    *p_dst = *p_src;
    p_dst++;
    p_src++;
  }

  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  // panic("Not implemented");
  assert(out != NULL);
  assert(in != NULL);

  char *p_out = out;
  const char* p_in = in;
  assert(p_in + n < p_out);
  assert(p_out + n < p_in);

  size_t i ;

  for(i=0; i<n; i++)
  {
    *p_out = *p_in ;
    p_out++;
    p_in++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  // panic("Not implemented");
  assert(s1 != NULL && s2 != NULL);
  const char *ps1 = s1;
  const char *ps2 = s2;

  while(n--)
  {
    if(*ps1 == *ps2)
    {
      ps1++;
      ps2++;
    }
    else
    {
      break;
    }
  }
  
  return (*ps1 - *ps2);
}

#endif
