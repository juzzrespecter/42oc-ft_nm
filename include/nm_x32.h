#ifndef _NM_X32_H
# define _NM_X32_H

// Definciones de cabeceras y símbolos ELF para 32

# include <stdint.h>
# ifndef EI_NIDENT
#  define EI_NIDENT 16
# endif

typedef uint16_t t_Elf32_Half;
typedef uint32_t t_Elf32_Word;
typedef uint32_t t_Elf32_Addr;
typedef uint32_t t_Elf32_Off;

typedef struct s_Elf32_Hdr
{
    unsigned char e_ident[EI_NIDENT];
    t_Elf32_Half e_type;
    t_Elf32_Half e_machine;
    t_Elf32_Word e_version;
    t_Elf32_Addr e_entry;
    t_Elf32_Off e_phoff;
    t_Elf32_Off e_shoff;
    t_Elf32_Word e_flags;
    t_Elf32_Half e_ehsize;
    t_Elf32_Half e_phentsize;
    t_Elf32_Half e_phnum;
    t_Elf32_Half e_shentsize;
    t_Elf32_Half e_shnum;
    t_Elf32_Half e_shstrndx;
} t_Elf32_Hdr;

typedef struct s_Elf32_Shdr
{
    t_Elf32_Word sh_name;
    t_Elf32_Word sh_type;
    t_Elf32_Word sh_flags;
    t_Elf32_Addr sh_addr;
    t_Elf32_Off sh_offset;
    t_Elf32_Word sh_size;
    t_Elf32_Word sh_link;
    t_Elf32_Word sh_info;
    t_Elf32_Word sh_addralign;
    t_Elf32_Word sh_entsize;
} t_Elf32_Shdr;

typedef struct s_Elf32_Sym
{
    t_Elf32_Word st_name;
    t_Elf32_Addr st_value;
    t_Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    t_Elf32_Half st_shndx;

} t_Elf32_Sym;

#endif //_NM_ELF32_H