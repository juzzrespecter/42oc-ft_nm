#ifndef NM_H
# define NM_H

# include <libft.h>
# include <fcntl.h>
# include <stdint.h>
# include <sys/stat.h>
# include <unistd.h>
# include <stdbool.h>
# include <stdio.h> // to remove

# define DEBUG_SYMS_F 1
# define EXTRN_ONLY_F (1<<1)
# define UNDEF_ONLY_F (1<<2)
# define NO_SORT_F    (1<<3)
# define REV_SORT_F   (1<<4)

typedef struct stat t_stat;

typedef enum e_retval
{
  NO_ERR = -1,
  ERR_NOSYM = 0,
  ERR_NOFILE = 1,
  ERR_DIR = 2,
  ERR_NOFMT = 3
} retval;

typedef struct s_context
{
  t_list *bin;
  char    flags;
} t_context;

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

// elf32 type definitions
typedef uint16_t t_Elf32_Half;
typedef uint32_t t_Elf32_Word;
typedef uint32_t t_Elf32_Addr;
typedef uint32_t t_Elf32_Off ;

// elf64 type definitions
typedef uint16_t t_Elf64_Half;
typedef uint32_t t_Elf64_Word;
typedef uint64_t t_Elf64_Addr;
typedef uint64_t t_Elf64_Off;
typedef uint64_t t_Elf64_Xword;

// estructura lista de simbolos
typedef struct s_Elf32_Sym
{
  t_Elf32_Word  st_name;
  t_Elf32_Addr  st_value;
  t_Elf32_Word  st_size;
  unsigned char st_info;
  unsigned char st_other;
  t_Elf32_Half  st_shndx;
} t_Elf32_Sym;

typedef struct s_Elf64_Sym
{
  t_Elf64_Word  st_name;
  unsigned char st_info;
  unsigned char st_other;
  t_Elf64_Half  st_shndx;
  t_Elf64_Xword st_size;
} t_Elf64_Sym;



typedef enum e_e_type
{
  ET_NONE = 0,        // No file type
  ET_REL = 1,         // Relocatable file
  ET_EXEC = 2,        // Executable file
  ET_DYN = 3,         // Shared object file
  ET_CORE = 4,        // Core file
  ET_LOPROC = 0xff00, // Processor-specific
  ET_HIPROC = 0xffff  // Processor-specific
} e_type;

typedef struct s_Elf32_Hdr
{
  unsigned char e_ident[EI_NIDENT];
  t_Elf32_Half  e_type;
  t_Elf32_Half  e_machine;
  t_Elf32_Word  e_version;
  t_Elf32_Addr  e_entry;
  t_Elf32_Off   e_phoff;
  t_Elf32_Off   e_shoff;
  t_Elf32_Word  e_flags;
  t_Elf32_Half  e_ehsize;
  t_Elf32_Half  e_phentsize;
  t_Elf32_Half  e_phnum;
  t_Elf32_Half  e_shentsize;
  t_Elf32_Half  e_shnum;
  t_Elf32_Half  e_shstrndx;
} t_Elf32_Hdr;

typedef struct s_Elf64_Hdr
{
  unsigned char e_ident[EI_NIDENT];
  t_Elf64_Half  e_type;
  t_Elf64_Half  e_machine;
  t_Elf64_Word  e_version;
  t_Elf64_Addr  e_entry;
  t_Elf64_Off   e_phoff;
  t_Elf64_Off   e_shoff;
  t_Elf64_Word  e_flags;
  t_Elf64_Half  e_ehsize;
  t_Elf64_Half  e_phentsize;
  t_Elf64_Half  e_phnum;
  t_Elf64_Half  e_shentsize;
  t_Elf64_Half  e_shnum;
  t_Elf64_Half  e_shstrndx;
} t_Elf64_Hdr;

typedef struct s_bin
{
  ei_class  class;
  ei_encode encoding;
  void*     elf_hdr;
  t_list*   symbol_lst;
} t_bin;

void   arg_parser(t_context*, int, char**);
retval parser_elf(int fd, t_context*);

int loader(t_context*);

void log_error(retval, char*);

#endif //NM_H