#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int dispinfo_fd = -1;

FILE *fb_device;

static int screen_w = 0, screen_h = 0;
static struct timeval now;
// 以毫秒为单位返回系统时间
uint32_t NDL_GetTicks() {
    gettimeofday(&now, NULL);

    return (now.tv_sec *1000 +  now.tv_usec/1000 );
}
// 读出一条事件信息, 将其写入`buf`中, 最长写入`len`字节
// 若读出了有效的事件, 函数返回1, 否则返回0
int NDL_PollEvent(char *buf, int len) {
 if (read(evtdev, buf, len) >0 )
  return 1;
return 0;
}

// bool parse_key_value(char *key_value_str, char *key, uint32_t *value)
// {
//   const char *delim = ":";
//   char *next_token;
//   assert( (key_value_str != NULL) && (key != NULL) && (value != NULL) );



// }
// 打开一张(*w) X (*h)的画布
// 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  else
  {
      char buf[64];
      int position = 0;


      printf("\nNDL_OpenCanvas : w = %d, h = %d \r\n" ,*w, *h);
      read(dispinfo_fd, buf, sizeof(buf) - 1);
      // for( i=0; i<sizeof(buf); i++)
      // {
      //   if(buf[i] == ":")
      //   {
      //     screen_w = atoi(buf)
      //   }
      // }
      // while()
      // {
      //   position ++;
      //   screen_w = 
      // }
      screen_w = 400; 
      screen_h = 300;
      // screen_w = (screen_w > *w ) ? *w : screen_w;
      // screen_h = (screen_h > *h ) ? *h : screen_h;
      printf("[NDL]buf = %s \r\n", buf);

  }
}

// 向画布`(x, y)`坐标处绘制`w*h`的矩形图像, 并将该绘制区域同步到屏幕上
// 图像像素按行优先方式存储在`pixels`中, 每个像素用32位整数以`00RRGGBB`的方式描述颜色
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int x_index;
  int y_index;
  uint32_t *pBuf = pixels;
  
  for( y_index =0; y_index < h; y_index++)
  {
    fseek(fb_device, (y_index + y + ) *  screen_w + x, SEEK_SET);
    fwrite(pBuf, sizeof(uint32_t), w, fb_device);
    pBuf += w;
  }

}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  else
  {
    evtdev = open("/dev/events", 0, 0);
    dispinfo_fd = open("/proc/dispinfo", 0, 0);
    // fbdev = open("/dev/fb", 0, 0);

    fb_device = fopen("/dev/fb", "w");
    
  }
  return 0;
}

void NDL_Quit() {
}
