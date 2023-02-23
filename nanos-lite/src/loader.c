#include <proc.h>
#include <elf.h>
#include "fs.h"

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
# define EXPECT_TYPE EM_X86_64          // see /usr/include/elf.h to get the right type
#elif defined(__ISA_MIPS32__)
# define EXPECT_TYPE ... 
#elif defined(__ISA_RISCV32__) || defined(__ISA_RISCV64__)
# define EXPECT_TYPE EM_RISCV
#else
# error Unsupported ISA
#endif

/*
  typedef struct {
      unsigned char e_ident[EI_NIDENT];
      uint16_t      e_type;
      uint16_t      e_machine;
      uint32_t      e_version;
      ElfN_Addr     e_entry;    //This member gives the virtual address to which the system first transfers control, thus starting the process.
      ElfN_Off      e_phoff;    //This  member holds the program header table's file offset in bytes.
      ElfN_Off      e_shoff;
      uint32_t      e_flags;
      uint16_t      e_ehsize;
      uint16_t      e_phentsize;// This member holds the size in bytes of one entry in the file's program header table;all entries are the same size.
      uint16_t      e_phnum;  // This member holds the number of entries in the program header table.
      uint16_t      e_shentsize;
      uint16_t      e_shnum;
      uint16_t      e_shstrndx;
  } ElfN_Ehdr;

  //Program header

  typedef struct {
    uint32_t   p_type;
    uint32_t   p_flags;
    Elf64_Off  p_offset; // This  member holds the offset from the beginning of the file at which the first byte of the segment resides.
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    uint64_t   p_filesz;
    uint64_t   p_memsz;
    uint64_t   p_align;
} Elf64_Phdr;
*/


extern size_t ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf  ;
  Elf_Phdr segment;
  uint32_t program_header_offset = 0;
  uint32_t i;
  int fd;

  fd = fs_open(filename, 0, 0);
  // ramdisk_read(&elf, 0, sizeof(Elf_Ehdr)/sizeof(char));
  fs_read(fd, &elf,  sizeof(Elf_Ehdr)/sizeof(char));

  assert(*(uint32_t *)elf.e_ident == 0x464c457f);
  assert(elf.e_machine == EXPECT_TYPE);  
  assert(elf.e_phentsize == sizeof(Elf_Phdr)/sizeof(char));
  printf("program header table's file offset is 0x%x ,number of entries is 0x%x  \r\n", elf.e_phoff, elf.e_phnum);

  for (i=0 ; i < elf.e_phnum; i++)
  {
    
    program_header_offset = elf.e_phoff + i * elf.e_phentsize ;
    printf(" program_header_offset is 0x%x \r\n", program_header_offset);
    fs_lseek(fd, program_header_offset, SEEK_SET);
    // ramdisk_read((void*)&segment, program_header_offset , elf.e_phentsize);
    fs_read(fd, (void*)&segment,elf.e_phentsize);
    printf("program segment #%d offset is 0x%x, VirtAddr is 0x%x ,PhysAddr is 0x%x , FileSiz is 0x%x, MemSiz is 0x%x \r\n", i, segment.p_offset,  \
                                                                                                      segment.p_vaddr, segment.p_paddr,  \
                                                                                                      segment.p_filesz, segment.p_memsz );   
    if(segment.p_type == PT_LOAD)
    {
      // ramdisk_read((void*)segment.p_vaddr, segment.p_offset, segment.p_filesz);
      fs_lseek(fd, segment.p_offset, SEEK_SET);
      fs_read(fd, (void*)segment.p_vaddr , segment.p_filesz);

      if (segment.p_memsz > segment.p_filesz)
      {
        memset((void*)(segment.p_vaddr + segment.p_filesz), 0, (segment.p_memsz - segment.p_filesz)/4 );
      }
    }
  }

  printf("system first transfers is 0x%x \r\n", elf.e_entry);
  
  return (uintptr_t)elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

