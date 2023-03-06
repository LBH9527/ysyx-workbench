#include <fs.h>
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len) ;
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t serial_write(const void *buf, size_t offset, size_t len) ;
extern size_t events_read(void *buf, size_t offset, size_t len) ;
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

// enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENT};
enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT};


size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}


/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write, 0},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write, 0},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write, 0},
  // [FD_UART]   = {"uart", 0, 0, invalid_read, serial_write, 0},
  // [FD_FB] = {"/dev/fb", 0, 0, invalid_read, invalid_write, 0},
  [FD_EVENT] = {"/dev/events", 0, 0, events_read, invalid_write, 0},

#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}


int fs_open(const char *pathname, int flags, int mode)
{
    uint32_t index = 0;
    bool is_find_file = false;

    for(index=0; index < sizeof(file_table)/sizeof(file_table[0]); index++)
    {
      if( memcmp( pathname, file_table[index].name, sizeof(file_table[index].name) ) == 0)
      {
        is_find_file = true;
        Log("[fs] fs open fd is 0x%x ,name is %s\n", index, file_table[index].name);
        break;
      }
    }
    assert(is_find_file == true);
    return index;
}

size_t fs_read(int fd, void *buf, size_t len)
{
    size_t read_length = 0; 
    size_t offset = 0;
    // 块设备
    if(file_table[fd].size != 0)
    {
      
      //printf("[fs] open_offset is 0x%x \n", file_table[fd].open_offset);
      // file-test 证明 ，assert 条件不成立
      //assert(file_table[fd].open_offset + len < file_table[fd].size);

      offset = file_table[fd].disk_offset + file_table[fd].open_offset;
      // printf("[fs] offset is 0x%x \n", offset);

      read_length = ramdisk_read(buf, offset, len);
      file_table[fd].open_offset += read_length;
      
    }
    else  // 字符设备
    {
      read_length = file_table[fd].read(buf, offset, len);
    }

    return read_length;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
  size_t offset = 0;
  size_t write_length = 0;

  if(fd >= FD_STDIN && fd <= FD_STDERR)
  {
      write_length = file_table[fd].write(buf, 0, len);
  }
  else
  {
    assert(file_table[fd].open_offset + len < file_table[fd].size);
    
    offset = file_table[fd].disk_offset + file_table[fd].open_offset;
    write_length = ramdisk_write(buf, offset, len);
    file_table[fd].open_offset += write_length;
  }

  return write_length;
}

#ifndef SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif

size_t fs_lseek(int fd, size_t offset, int whence)
{
  //printf("fd is: 0x%x, offset is %d, whence is %d\n", fd, offset, whence);
  switch(whence)
  {
    case SEEK_SET : file_table[fd].open_offset = offset; break;
    case SEEK_CUR : file_table[fd].open_offset += offset; break;
    case SEEK_END : file_table[fd].open_offset = file_table[fd].size + offset; break;
    default : assert(0); break;
  }
  //  file-test 证明 ，assert 条件不成立
  // assert(file_table[fd].open_offset <= file_table[fd].size);

  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
  file_table[fd].open_offset = 0;

  return 0;
}