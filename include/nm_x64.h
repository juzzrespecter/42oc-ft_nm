#ifndef _NM_X64_H
# define _NM_X64_H

# include <stdint.h>
# ifndef EI_NIDENT
#  define EI_NIDENT 16
# endif

// elf64 type definitions
typedef uint16_t t_Elf64_Half;
typedef uint32_t t_Elf64_Word;
typedef uint64_t t_Elf64_Addr;
typedef uint64_t t_Elf64_Off;
typedef uint64_t t_Elf64_Xword;

typedef struct s_Elf64_Hdr
{
    unsigned char e_ident[EI_NIDENT];
    t_Elf64_Half e_type;
    t_Elf64_Half e_machine;
    t_Elf64_Word e_version;
    t_Elf64_Addr e_entry;
    t_Elf64_Off e_phoff;
    t_Elf64_Off e_shoff;
    t_Elf64_Word e_flags;
    t_Elf64_Half e_ehsize;
    t_Elf64_Half e_phentsize;
    t_Elf64_Half e_phnum;
    t_Elf64_Half e_shentsize;
    t_Elf64_Half e_shnum;
    t_Elf64_Half e_shstrndx;
} t_Elf64_Hdr;

typedef struct s_Elf64_Shdr
{
    t_Elf64_Word sh_name;
    t_Elf64_Word sh_type;
    t_Elf64_Xword sh_flags;
    t_Elf64_Addr sh_addr;
    t_Elf64_Off sh_offset;
    t_Elf64_Xword sh_size;
    t_Elf64_Word sh_link;
    t_Elf64_Word sh_info;
    t_Elf64_Xword sh_addralign;
    t_Elf64_Xword sh_entsize;
} t_Elf64_Shdr;

typedef struct s_Elf64_Sym
{
    t_Elf64_Word st_name;
    unsigned char st_info;
    unsigned char st_other;
    t_Elf64_Half st_shndx;
    t_Elf64_Addr st_value;
    t_Elf64_Xword st_size;
} t_Elf64_Sym;

# define ELF64_ST_BIND(info)    ((info) >> 4)
# define ELF64_ST_TYPE(info)    ((info) & 0xf)

# define ELF64_ST_VISIBILITY(oth)    ((oth) & 0x3)

void print_section_values_x64(t_Elf64_Shdr*, int);
void print_symbol_table_x64(t_Elf64_Sym*, int);

# endif //_NM_X64_h

