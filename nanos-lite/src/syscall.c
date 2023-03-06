#include <common.h>
#include "syscall.h"
#include "fs.h"
#include <time.h>

#define STRACE_ENABLE   0

const char *syscall_name[] = {
   "exit", "yield", "open", "read", "write", "kill", "getpid", "close","lseek" ,"brk",
   "fstat","time","signal","execve","fork","link","unlink","wait","times","gettimeofday"
};

int do_SYS_open(const char *pathname, int flags, int mode);
size_t do_SYS_read(int fd, void *buf, size_t len);
size_t do_SYS_write(int fd, const void *buf, size_t count);
int do_SYS_close(int fd);
size_t do_SYS_lseek(int fd, size_t offset, int whence);
int do_SYS_gettimeofday(void *pTv );


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
#if (STRACE_ENABLE)
  printf("strace : %s(0x%x, 0x%x, 0x%x) ", syscall_name[c->GPR1], c->GPR2, c->GPR3, c->GPR4 );
#endif
  switch (a[0]) {
    case SYS_exit: halt(c->GPR2) ; break;
    case SYS_yield: yield(); c->GPR2 = 0; break;    
    case SYS_open: c->GPR2 = do_SYS_open((const char *)c->GPR2, c->GPR3, c->GPR4) ;break;
    case SYS_read: c->GPR2 = do_SYS_read(c->GPR2,  (void*)c->GPR3, c->GPR4) ;break;
    case SYS_write: c->GPR2 = do_SYS_write(c->GPR2, (void*)(c->GPR3), c->GPR4); break;
    case SYS_brk: c->GPR2 = 0; break;
    case SYS_close: c->GPR2 = do_SYS_close(c->GPR2); break;
    case SYS_lseek: c->GPR2 = do_SYS_lseek(c->GPR2, c->GPR3, c->GPR4); break;
    case SYS_gettimeofday : c->GPR2 = do_SYS_gettimeofday((void *)c->GPR2); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
#if (STRACE_ENABLE)
  printf("= %d\r\n", c->GPR2);
#endif
}

int do_SYS_open(const char *pathname, int flags, int mode)
{
  return fs_open(pathname, flags, mode) ;
}

size_t do_SYS_read(int fd, void *buf, size_t len)
{
  return fs_read(fd, buf, len) ;

}

size_t do_SYS_write(int fd, const void *buf, size_t count)
{
  return(fs_write(fd, buf, count));    
}

size_t do_SYS_lseek(int fd, size_t offset, int whence) 
{
  return fs_lseek(fd, offset, whence);
}

int do_SYS_close(int fd)
{
  return fs_close(fd) ;
}

int do_SYS_gettimeofday(void *pTv)
{
    struct timeval *p = (struct timeval *)pTv;

    p->tv_usec =  io_read(AM_TIMER_UPTIME).us % 1000000;
    p->tv_sec =  io_read(AM_TIMER_UPTIME).us /1000000;
    
    
    return 0;
}
