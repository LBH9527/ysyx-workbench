#include "trace.h"
#include <libgen.h>     //for dirname

#undef DBG_TAG
#undef DBG_LVL
#define DBG_TAG          "ftrace"
#define DBG_LVL          DBG_LOG
#include <debug_log.h> 

/*
1. 从 elf header 中得到 start of section header ，Size of section headers ， Number of section headers 
2. 从 section table 找到 符号表（symbol table） 信息
3. 符号表中得到属性位 FUNC 的表项，表项中还有符号的起始地址和大小
4. 根据地址就能得到调用的函数
*/

#define EI_NIDENT 16

typedef word_t ElfN_Addr;
typedef word_t ElfN_Off;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    ElfN_Addr     e_entry;
    ElfN_Off      e_phoff;
    ElfN_Off      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} ElfN_Ehdr;

/* Legal values for sh_type (section type).  */
#define SHT_NULL          0             /* Section header table entry unused */
#define SHT_PROGBITS      1             /* Program data */
#define SHT_SYMTAB        2             /* Symbol table */
#define SHT_STRTAB        3             /* String table */

typedef struct {
    uint32_t   sh_name;
    uint32_t   sh_type;
    word_t   sh_flags;
    word_t sh_addr;
    word_t  sh_offset;
    word_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    word_t   sh_addralign;
    word_t   sh_entsize;
} Elfn_Shdr;

/* How to extract and insert information held in the st_info field.  */

#define ELF32_ST_BIND(val)              (((unsigned char) (val)) >> 4)
#define ELF32_ST_TYPE(val)              ((val) & 0xf)
#define ELF32_ST_INFO(bind, type)       (((bind) << 4) + ((type) & 0xf))

/* Both Elf32_Sym and Elf64_Sym use the same one-byte st_info field.  */
#define ELF64_ST_BIND(val)              ELF32_ST_BIND (val)
#define ELF64_ST_TYPE(val)              ELF32_ST_TYPE (val)
#define ELF64_ST_INFO(bind, type)       ELF32_ST_INFO ((bind), (type))

/* Legal values for ST_TYPE subfield of st_info (symbol type).  */

#define STT_NOTYPE      0               /* Symbol type is unspecified */
#define STT_OBJECT      1               /* Symbol is a data object */
#define STT_FUNC        2               /* Symbol is a code object */
#define STT_SECTION     3               /* Symbol associated with a section */
#define STT_FILE        4               /* Symbol's name is file name */
#define STT_COMMON      5               /* Symbol is a common data object */
#define STT_TLS         6               /* Symbol is thread-local data object*/
#define STT_NUM         7               /* Number of defined types.  */
#define STT_LOOS        10              /* Start of OS-specific */
#define STT_GNU_IFUNC   10              /* Symbol is indirect code object */
#define STT_HIOS        12              /* End of OS-specific */
#define STT_LOPROC      13              /* Start of processor-specific */
#define STT_HIPROC      15              /* End of processor-specific */

typedef struct {
uint32_t      st_name;
ElfN_Addr    st_value;
uint32_t      st_size;
unsigned char st_info;
unsigned char st_other;
uint16_t      st_shndx;
} Elf32_Sym;

typedef struct {
uint32_t      st_name;
unsigned char st_info;
unsigned char st_other;
uint16_t      st_shndx;
ElfN_Addr    st_value;
uint64_t      st_size;
} Elf64_Sym;


FILE *elf_fp = NULL;
char *ftrace_log_file = NULL;
FILE *ftrace_log_fp = NULL;

typedef struct 
{
    char *table ;
    word_t sh_size;
    word_t sh_entsize;
    char *name_str;
}symbol_info_t;

symbol_info_t symbol_info = {
    .table = NULL,
    .sh_size = 0,
    .sh_entsize = 0,
    .name_str = NULL,
};


void init_ftrace(const char *image_elf)
{
    size_t ret_code;
    uint32_t i;
    ElfN_Ehdr header ;
    ElfN_Addr start_addr_of_strtab = 0;
    ElfN_Addr start_addr_of_symtab = 0;

    Log("Function Trace: %s", MUXDEF(CONFIG_FTRACE, ANSI_FMT("ON", ANSI_FG_GREEN), ANSI_FMT("OFF", ANSI_FG_RED)));

    if (image_elf != NULL)
    {
        FILE *fp = fopen(image_elf, "r");
        Assert(fp, "Can not open '%s'", image_elf);
        elf_fp = fp;
    }
    Log("Read elf is from %s", image_elf ? image_elf : "stdout");
    ftrace_log_file = strcat(dirname((char*)image_elf), "/function_trace.txt");
    if(ftrace_log_file != NULL)
    {
        FILE *fp = fopen(ftrace_log_file, "w");
        Assert(fp, "Can not open '%s'", ftrace_log_file);
        ftrace_log_fp = fp;
    }
    Log("Function trace is written to %s", ftrace_log_file ? ftrace_log_file : "stdout");

    ret_code = fread(&header, 1, sizeof(ElfN_Ehdr), elf_fp);
    if (ret_code >=  sizeof(ElfN_Ehdr))
    {
        LOG_D("Start of section headers : %ld", header.e_shoff);
        LOG_D("Size of section headers  : %d", header.e_shentsize);
        LOG_D("Number of section headers: %d", header.e_shnum);
        assert(sizeof(Elfn_Shdr) == header.e_shentsize);

    }
    Elfn_Shdr pShdr;
    Elfn_Shdr pShdr_of_symtab ;
    Elfn_Shdr pShdr_of_strtab ;
    bool find_symtab = false;
    bool find_strtab = false;

    memset(&pShdr_of_symtab, 0, sizeof(pShdr_of_symtab));
    memset(&pShdr_of_strtab, 0, sizeof(pShdr_of_strtab));

    fseek(elf_fp, header.e_shoff, SEEK_SET);
    for(i = 0; i<header.e_shnum; i++)
    {
        ret_code = fread(&pShdr, 1, header.e_shentsize, elf_fp);
        assert(ret_code == header.e_shentsize);
        if (pShdr.sh_type == SHT_SYMTAB)
        {
            find_symtab = true;
            memcpy(&pShdr_of_symtab , &pShdr, sizeof(pShdr));
        }
        else if(pShdr.sh_type == SHT_STRTAB)
        {
            memcpy(&pShdr_of_strtab , &pShdr, sizeof(pShdr));
            find_strtab = true;
        }
        if (find_strtab && find_symtab )
            break;
        
    }
    assert(find_strtab == true);
    assert(find_symtab == true);
    assert((pShdr_of_symtab.sh_size % pShdr_of_symtab.sh_entsize) == 0);
    assert(pShdr_of_symtab.sh_entsize == sizeof(Elf64_Sym) );
    // symbol table 
    start_addr_of_symtab = pShdr_of_symtab.sh_addr + pShdr_of_symtab.sh_offset;
  
    symbol_info.sh_size = pShdr_of_symtab.sh_size ;
    symbol_info.sh_entsize = pShdr_of_symtab.sh_entsize; 
    symbol_info.table = malloc(pShdr_of_symtab.sh_size);
    assert(symbol_info.table != NULL);
    fseek(elf_fp, start_addr_of_symtab, SEEK_SET);
    ret_code = fread(symbol_info.table, 1, pShdr_of_symtab.sh_size, elf_fp) ;
    assert(ret_code == pShdr_of_symtab.sh_size); 
    // string table
    start_addr_of_strtab = pShdr_of_strtab.sh_addr + pShdr_of_strtab.sh_offset;    
    symbol_info.name_str = malloc(pShdr_of_strtab.sh_size);    
    assert(symbol_info.name_str != NULL); 
    fseek(elf_fp, start_addr_of_strtab, SEEK_SET);  
    ret_code = fread(symbol_info.name_str, 1, pShdr_of_strtab.sh_size, elf_fp);
    assert(ret_code == pShdr_of_strtab.sh_size);

    // printf func adder and name
    Elf64_Sym *pSym;    
    for(i=0; i < symbol_info.sh_size/symbol_info.sh_entsize ;i++) 
    {
        pSym = (Elf64_Sym *)(symbol_info.table + sizeof(Elf64_Sym) * i);
        if ( ELF64_ST_TYPE(pSym->st_info) == STT_FUNC )
        {
            LOG_D("fun addr[0x%016lx, 0x%016lx),  func name : %s", pSym->st_value, pSym->st_value + pSym->st_size, (char*)(symbol_info.name_str + pSym->st_name));
                
        }
    }

}
static uint32_t file_line = 0;
#define TRACE_LOG_MAX_LINE  1000


int function_trace(word_t pc, word_t addr, bool is_call)
{
    uint32_t i;
    Elf64_Sym *pSym;    
    bool is_find_func = false;
    char *jump_type_str;
    int32_t j;
    static int32_t call_level = 0;

    assert(ftrace_log_fp != NULL);
    if(file_line++ > TRACE_LOG_MAX_LINE)
    {
        return 0;
    }
    for(i=0; i < symbol_info.sh_size/symbol_info.sh_entsize ;i++) 
    {
        pSym = (Elf64_Sym *)(symbol_info.table + sizeof(Elf64_Sym) * i);

        if ( ( ELF64_ST_TYPE(pSym->st_info) == STT_FUNC ) && ( (addr >= pSym->st_value) && (addr < (pSym->st_value + pSym->st_size))) )
        {
            is_find_func = true;

            fprintf(ftrace_log_fp, "[#%03d.][0x%016lx]:",file_line, pc);
            (is_call == true) ? call_level++ : call_level--;
            
            if (call_level < 0)
                call_level = 0;
            for(j=0; j<call_level; j++)
                fprintf(ftrace_log_fp, "  ");
            
            jump_type_str = (is_call == true) ? "call" : "ret";

            fprintf(ftrace_log_fp, "%s", jump_type_str);
            fprintf(ftrace_log_fp, "[%s@0x%016lx]\r\n", (char*)(symbol_info.name_str + pSym->st_name), addr);

        }
    }

    assert (is_find_func == true);
    return is_find_func;
}

int reinit_ftrace(void)
{
    free(symbol_info.table);
    free(symbol_info.name_str);
    fclose(elf_fp);
    return 0;
}
