#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>        // for bool
#include <assert.h>

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

static void remove_str_space(char *dest, const char *str)
{
    const char *pStr; 
    char *pDest;

    assert(str != NULL);
    assert(dest != NULL);
    pStr = str;
    pDest = dest;

    while(*pStr != 0)
    {
        if(*pStr != ' ')
        {
            *pDest = *pStr;
            pDest++;
        }

        pStr++;
    }
}

#define KEY_VLAUE_SIZE 64
bool parse_key_value(char *key_value_str, char *key, uint32_t *value)
{
    const char *delim = ":\n";
    bool is_find = false;

    char trimStr[KEY_VLAUE_SIZE]; 
    // printf("KEY is %s\n", key);
    assert( (key_value_str != NULL) && (key != NULL) && (value != NULL) );    
    assert(strlen(key_value_str) <= (KEY_VLAUE_SIZE -1) );
    //去掉字符串中的空格
    remove_str_space(trimStr, key_value_str);

    char *token = strtok(trimStr, delim);
    if (token == NULL) {
        printf ("key / value string is null.");
        return false;
    }

    while(token) {
        if(is_find == true)
        {
            *value = atoi(token);
            break;
        }
        else if(strcmp(token, key) == 0)
        {
            is_find = true;
        }
        token = strtok(NULL, delim);    // 获取下个 token
    }

    if(is_find)
        return true;
    return false;
}


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

      if(parse_key_value(buf, "WIDTH", &screen_w) )
      {
          printf("[WIDTH] : %d\n", screen_w);
      }
      else
      {
          printf("Didn't find key of [WIDTH] \n");
          assert(0);
      }

      if(parse_key_value(buf, "HEIGHT", &screen_h) )
      {
          printf("[HEIGHT] : %d\n", screen_h);
      }
      else
      {
          printf("Didn't find key of [HEIGHT] \n");
          assert(0);
      }
      if (*w > screen_w)
        *w = screen_w;
      if (*h > screen_h)
        *h = screen_h;
  }
}

// 向画布`(x, y)`坐标处绘制`w*h`的矩形图像, 并将该绘制区域同步到屏幕上
// 图像像素按行优先方式存储在`pixels`中, 每个像素用32位整数以`00RRGGBB`的方式描述颜色
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int x_index;
  int y_index;
  uint32_t *pBuf = pixels;
  uint32_t x_offset = (screen_w - w) / 2 + y;
  uint32_t y_offset = (screen_h - h) / 2 + x;
  // uint32_t x_offset = y;
  // uint32_t y_offset = x;


  for( y_index =0; y_index < h; y_index++)
  {
    // printf("y_index = %d, w = %d\n", y_index, w);

    fseek(fb_device, (y_index + y_offset) *  screen_w + x_offset, SEEK_SET);

    fwrite(pBuf, sizeof(uint32_t), w, fb_device);
    fflush(fb_device);    //writes any unwritten data from the stream's buffer to the associated output device.
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
