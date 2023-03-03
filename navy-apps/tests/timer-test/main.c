#include <unistd.h>
#include <stdio.h>
// #include "NDL.h"

#if 0
static int get_cur_time_ms(void)
{
    struct timeval now;

    gettimeofday(&now, NULL);

    return (now.tv_sec *1000 +  now.tv_usec/1000 );
}
#endif

int main() 
{
    int boot_time = NDL_GetTicks();
    int now;
    int index = 0;
    while(1)
    {
        // now = get_cur_time_ms();
        now = NDL_GetTicks();
        if(now - boot_time >= 500)
        {
            boot_time = now;
            
            printf("500 milliseconds have passed #%d\r\n", index++);
        }
    }
  return 0;
}
