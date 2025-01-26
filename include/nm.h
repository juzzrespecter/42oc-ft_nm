#ifndef NM_H
# define NM_H

# include "../libft/includes/libft.h"
# include <sys/mman.h>
# include <stdio.h>
# include <fcntl.h>
# include <stdint.h>
# include <stdlib.h>
# include <sys/stat.h>
# include <unistd.h>
# include <stdbool.h>
# include "debug.h"

# define DEBUG_SYMS_F 1
# define EXTRN_ONLY_F (1<<1)
# define UNDEF_ONLY_F (1<<2)
# define NO_SORT_F    (1<<3)
# define REV_SORT_F   (1<<4)

typedef struct stat t_stat;

typedef enum e_error
{
  NO_ERR = -1,
  ERR_NOSYM = 0,
  ERR_NO_FILE = 1,
  ERR_IS_DIR = 2,
  ERR_NO_FORMAT = 3,
  ERR_SYS = 4
} error;

/* ELF file identification */
# define EI_MAG0 0
# define EI_MAG1 1
# define EI_MAG2 2
# define EI_MAG3 3
# define EI_CLASS 4
# define EI_DATA 5
# define EI_VERSION 6

# define EI_NIDENT 16

enum e_elfmag
{
  ELFMAG0 = 0x7f,
  ELFMAG1 = 0x45,
  ELFMAG2 = 0x4c,
  ELFMAG3 = 0x46
};

typedef enum e_ei_class
{
  ELFCLASSNONE = 0,
  ELFCLASS32 = 1,
  ELFCLASS64 = 2
} ei_class;

typedef enum e_ei_encode
{
  ELFDATANONE = 0,
  ELFDATA2LSB = 1,
  ELFDATA2MSB = 2
} ei_encode;

# define ELF64_ST_BIND(info)    ((info) >> 4)
# define ELF64_ST_TYPE(info)    ((info) & 0xf)

# define ELF64_ST_VISIBILITY(oth)    ((oth) & 0x3)

/* https://refspecs.linuxfoundation.org/elf/gabi4+/ch4.intro.html#data_representation */

// elf32 type definitions
typedef uint16_t t_Elf32_Half;
typedef uint32_t t_Elf32_Word;
typedef uint32_t t_Elf32_Addr;
typedef uint32_t t_Elf32_Off;

// elf64 type definitions
typedef uint16_t t_Elf64_Half;
typedef uint32_t t_Elf64_Word;
typedef uint64_t t_Elf64_Addr;
typedef uint64_t t_Elf64_Off;
typedef uint64_t t_Elf64_Xword;

typedef enum e_e_type
{
  ET_NONE = 0, // No file type
  ET_REL = 1, // Relocatable file
  ET_EXEC = 2, // Executable file
  ET_DYN = 3, // Shared object file
  ET_CORE = 4, // Core file
  ET_LOPROC = 0xff00, // Processor-specific
  ET_HIPROC = 0xffff // Processor-specific
} e_type;


/* https://refspecs.linuxbase.org/elf/gabi4+/ch4.sheader.html */

typedef enum e_sh_type
{
  SHT_NULL = 0, // Inactive section header
  SHT_PROGBITS = 1, // Defined by program
  SHT_SYMTAB = 2, // Symbol table (symbols for link editing)
  SHT_STRTAB = 3, // String table
  SHT_RELA = 4, // Relocation entries
  SHT_HASH = 5, // Symbol hash table
  SHT_DYNAMIC = 6, // Information for dynamic linking
  SHT_NOTE = 7, // Information mark file
  SHT_NOBITS = 8, // Section occupies no space in file
  SHT_REL = 9, // Relocation entries w/o explicit addend
  SHT_SHLIB = 10, // Reserved
  SHT_DYNSYM = 11, // Symbol table (dynamic linking)
  SHT_INIT_ARRAY = 14, // Array of pointers to initialization functions
  SHT_FINI_ARRAY = 15, // Array of pointers to termination functions
  SHT_PREINIT_ARRAY = 16, // Array of pointers to functions that are invoked before all other init. functions
  SHT_GROUP = 17, // Defines a section group
  SHT_SYMTAB_SHNDX = 18, // Associated with SHT_SYMTAB section
  SHT_LOPROC = 0x70000000, // Processor specific semantics
  SHT_HIPROC = 0x7fffffff, // ...
  SHT_LOUSER = 0x80000000, // Lower bound of indexes reserved for application programs
  SHT_HIUSER = 0xffffffff // Higher bound of above
} sh_type;

typedef enum e_sh_flags
{
  SHF_WRITE = 1, // Data writable during process exec.
  SHF_ALLOC = (1 << 1), // Section occupies memory during process exec.
  SHF_EXECINSTR = (1 << 2), // Executable machine isntructions
  SHF_MERGE = (1 << 5), // May be merged to eliminate duplication
  SHF_STRING = (1 << 6), // Data elements in section consist of null-terminated char. strings
  SHF_INFO_LINK = (1 << 7), // sh_info holds a section header table index
  SHF_LINK_ORDER = (1 << 8), // Special order requirements for linking
  SHF_OS_NONCONFORMING = (1 << 9),
  // Special OS_specific processing (sh_type, sh_flags in OS-specific ranges validation)
  SHF_GROUP = (1 << 10), // Member of a section group (e_type must be set to ET_REL)
  SHF_TLS = (1 << 11), // Holds thread-localstorage
  SHF_MASKOS = 0x0ff00000, // Reserved for os-specific semantics
  SHF_MASKPROC = 0xf0000000 // Reserved for processor-specific semantics
} sh_flags;

typedef enum e_st_bind
{
  STB_LOCAL = 0, // Symbol is local (not visible outside object)
  STB_GLOBAL = 1, // Symbol is global (satisfies all file's reference)
  STB_WEAK = 2, // Symbol is weak (lower precedence than local)
  STB_LOOS = 10, // Lower bound for OS-specific semantics
  STB_HIOS = 12, // Higher bound for OS-specific semantics
  STB_LOPROC = 13, // Lower bound for processor-specific semantics
  STB_HIPROC = 15 // Higher bound for porcessor-specific semantics
} st_bind;

typedef enum e_st_type
{
  STT_NOTYPE = 0, // Symbol type not specified
  STT_OBJECT = 1, // Symbol associated with a data object
  STT_FUNC = 2, // Symbol associated with a function
  STT_SECTION = 3, // Symbol associated with a section
  STT_FILE = 4, // Gives the name of the source file associated
  STT_COMMON = 5, // Labels an uninitialized common block
  STT_TLS = 6, // Symbol specifies a Thread-Local-Storage entity
  STT_LOOS = 10, // Reserved for OS
  STT_HIOS = 12, // ""
  STT_LOPROC = 13, // Reserved for processor
  STT_HIPROC = 15 // ""
} st_type;

// Special section indexes
typedef enum e_st_shndx
{
  SHN_UNDEF = 0,
  SHN_LORESERVE = 0xff00,
  SHN_LOPROC = 0xff00,
  SHN_BEFORE = 0xff00,
  SHN_AFTER = 0xff01,
  SHN_AMD64_LCOMMON = 0xff02,
  SHN_HIPROC = 0xff1f,
  SHN_LOOS = 0xff20,
  SHN_LOSUNW = 0xff3f,
  SHN_SUNW_IGNORE = 0xff3f,
  SHN_HISUNW = 0xff3f,
  SHN_HIOS = 0xff3f,
  SHN_ABS = 0xfff1,
  SHN_COMMON = 0xfff2,
  SHN_XINDEX = 0xffff,
  SHN_HIRESERVE = 0xffff
} st_shndx;

typedef struct s_symbol
{
  void* sym_ptr;
  char* sym_name;
  char sym_type;
} t_symbol;

typedef struct s_Elf_Sym_wrapper
{
  uint32_t sh_link;
  void* sym;
} t_Elf_Sym_wrapper;

typedef struct s_strtab
{
  size_t strtab_index;
  char* strtab;
} t_strtab;

typedef struct s_bin
{
  char* b_path;
  void* b_src;
  int b_fd;
  size_t b_size;

  ei_class b_class;
  ei_encode b_encoding;
  uint32_t b_version;

  uint64_t shoff;
  uint16_t shnum;
  uint16_t shentsize;
  uint16_t shstrndx;

  t_list* b_elf_shdr;
  t_list* b_strtab_lst;
  t_list* b_sym_lst;
  t_list* b_nm_sym_lst;
} t_bin;

typedef struct s_nm
{
  t_list* b_lst;
  char flags;
  int state;
} t_nm;

void arg_parser(t_nm*, char**);
void parser_elf(t_bin*, t_nm*);

char* select_strtab(size_t, t_bin*);

void parser_elf_hdr_x32(t_bin*, t_nm*);
void parse_symbols_to_nm_fmt_x32(t_bin*, t_nm*);

void parser_elf_hdr_x64(t_bin*, t_nm*);
void parse_symbols_to_nm_fmt_x64(t_bin*, t_nm*);

t_list* build_new_shdr_node(void*, ei_class, t_nm*);
t_list *build_new_sym_node(void *, ei_class, uint32_t , t_nm *);

void output_nm_symbols(t_bin*, t_nm*);

void log_error(error, char*);
int clean_context(t_nm*);
void log_and_exit(error, char*, t_nm*);

#endif //NM_H
