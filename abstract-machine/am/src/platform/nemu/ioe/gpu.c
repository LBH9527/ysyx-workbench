#include <am.h>
#include <nemu.h>
#include "klib.h"

#define SCREEN_HEIGHT_ADDR    (VGACTL_ADDR)
#define SCREEN_WIDTH_ADDR     (VGACTL_ADDR + 2)

#define SYNC_ADDR (VGACTL_ADDR + 4)

int width;
int height;

void __am_gpu_init() {
  int i = 0;
  int x_index;
  int y_index;

  width = inw(SCREEN_WIDTH_ADDR);  // TODO: get the correct width
  height = inw(SCREEN_HEIGHT_ADDR);  // TODO: get the correct height

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < width * height ; i ++) 
    fb[i] = 0x00FF0000;
  outl(SYNC_ADDR, 1);

  i = 0;
  for (y_index = height/4; y_index < height/2; y_index++)
  {
    for (x_index = width/4 ; x_index < width/2; x_index++)
    {
        fb[x_index + y_index * width] = 0x0000E676 ;
    }
  }
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, 
    .has_accel = false,
    .width = inw(SCREEN_WIDTH_ADDR), 
    .height = inw(SCREEN_HEIGHT_ADDR),
    .vmemsz = 0
  };
}
/*
typedef struct {
  int x, y;		//屏幕`(x, y)`坐标处
  void *pixels;	//图像像素按行优先方式存储在`pixels`中,
  int w, h;		//绘制`w*h`的矩形图像
  _Bool sync;	//若`sync`为`true`, 则马上将帧缓冲中的内容同步到屏幕上.
} AM_GPU_FBDRAW_T;
*/
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // int i;
  int x_index;
  int y_index;

  // printf("[gpu.c]width = %d, height = %d \r\n",width, height);
  // printf("ctl->x = %d, ctl->y = %d, ctl->w = %d, ctl->h = %d \r\n", ctl->x, ctl->y, ctl->w, ctl->h);

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;

  for (y_index = ctl->y; y_index < ctl->y + ctl->h; y_index++)
  {
    for (x_index = ctl->x ; x_index < ctl->x + ctl->w; x_index++)
    {
        fb[x_index + y_index * width] = *(uint32_t *)ctl->pixels ;
        ctl->pixels += sizeof(uint32_t);
    }
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
