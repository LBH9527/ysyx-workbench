#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {

  const char *p = buf;
  size_t i;

  for(i=0; i<len; i++)
  {
    putch(*p);
    p++;
  }

  return i;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  char *pBuf = buf;
  
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);

  if (ev.keycode != AM_KEY_NONE) 
  {
      if(ev.keydown == true)
      {
        len = sprintf(pBuf, "kd %s", keyname[ev.keycode]);
      }
      else
      {
        len = sprintf(pBuf, "ku %s", keyname[ev.keycode]);
      }
      // printf("[device] event read key code %d,  %s\n", ev.keycode,  pBuf );
  }
  else
  {
      len = 0;
  }
  
  return len;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  char *pBuf = buf;
  size_t sz = 0;
  
  sz  = snprintf((char *)pBuf, len, "WIDTH: %d\nHEIGHT: %d\n", io_read(AM_GPU_CONFIG).width, io_read(AM_GPU_CONFIG).height);
  return sz;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {

  uint32_t width = io_read(AM_GPU_CONFIG).width;
  // uint32_t height = io_read(AM_GPU_CONFIG).height;
  uint32_t x = offset  % width;
  uint32_t y = offset / width;
  uint32_t w = len % width; 
  uint32_t h = len / width;
  io_write(AM_GPU_FBDRAW, x, y, (uint32_t*)buf, w, h, true);
  
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
