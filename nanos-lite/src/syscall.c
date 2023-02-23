#include <common.h>
#include "syscall.h"
#include "fs.h"

#define STRACE_ENABLE   1

const char *syscall_name[] = {
   "exit", "yield", "open", "read", "write", "kill", "getpid", "close","lseek" ,"brk",
   "fstat","time","signal","execve","fork","link","unlink","wait","times","gettimeofday"
};

static int do_SYS_open(const char *pathname, int flags, int mode);
static size_t do_SYS_read(int fd, void *buf, size_t len);
static size_t do_SYS_write(int fd, const void *buf, size_t count);
static int do_SYS_close(int fd);
static size_t do_SYS_lseek(int fd, size_t offset, int whence);

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
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
#if (STRACE_ENABLE)
  printf("= %d\r\n", c->GPR2);
#endif
}

static int do_SYS_open(const char *pathname, int flags, int mode)
{
  return fs_open(pathname, flags, mode) ;
}

static size_t do_SYS_read(int fd, void *buf, size_t len)
{
  return fs_read(fd, buf, len) ;

}

static size_t do_SYS_write(int fd, const void *buf, size_t count)
{
  return(fs_write(fd, buf, count));    
}

static size_t do_SYS_lseek(int fd, size_t offset, int whence) 
{
  return fs_lseek(fd, offset, whence);
}

static int do_SYS_close(int fd)
{
  return fs_close(fd) ;
}