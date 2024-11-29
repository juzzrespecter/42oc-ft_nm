#include "../include/nm.h"

/**
* First ELF parser step, identity part of ELF header validator.
*
* Validates binary's magic number and class and encoding fields.
* Returns true if any identity field has an invalid value.
*/
static bool __parser_identity_fields(char *ei_ident)
{
  return (ei_ident[EI_MAG0] != ELFMAG0 ||
      ei_ident[EI_MAG1] != ELFMAG1 ||
      ei_ident[EI_MAG2] != ELFMAG2 ||
      ei_ident[EI_MAG3] != ELFMAG3 ||
      (ei_ident[EI_CLASS] != ELFCLASS32 && ei_ident[EI_CLASS] != ELFCLASS64) ||
      (ei_ident[EI_DATA] != ELFDATA2LSB && ei_ident[EI_DATA] != ELFDATA2MSB));
}

//* Saves into bin context structure parsing values.

static bool __parser_elf_header_ident(t_bin* bin, t_nm *ctx)
{
  char    ei_ident[EI_NIDENT];
  ssize_t rd;

  rd = read(bin->b_fd, ei_ident, EI_NIDENT);
  if (rd < EI_NIDENT)
  {
     if (rd < 0)
       log_and_exit(ERR_SYS, NULL, ctx);
     else
     {
        log_error(ERR_NO_FORMAT, bin->b_path);
        return false;
     }
  }
  if (__parser_identity_fields(ei_ident))
  {
    log_error(ERR_NO_FORMAT, bin->b_path);
    return false;
  }
  bin->b_class = ei_ident[EI_CLASS];
  bin->b_encoding = ei_ident[EI_DATA];
  bin->b_version = ei_ident[EI_VERSION];

  return true;
}

/* de momento txdo en 64, pensar forma de generalizar */

/**
*  ELF file header parser.
*
* @param bin: current binary context.
* @param ctx: program context.
*/
void parser_elf(t_bin* bin, t_nm* ctx)
{
  void     *hdr_ptr;
  uint32_t  version;

  if (!__parser_elf_header_ident(bin, ctx))
    return ;
  hdr_ptr = mmap((void *)0, sizeof(t_Elf64_Hdr), PROT_WRITE, MAP_PRIVATE, bin->b_fd, 0);
  if (hdr_ptr == MAP_FAILED)
    log_and_exit(ERR_SYS, NULL, ctx);

  version = ((t_Elf64_Hdr *)hdr_ptr)->e_version;

  bin->shoff = ((t_Elf64_Hdr *)hdr_ptr)->e_shoff;
  bin->shnum = ((t_Elf64_Hdr *)hdr_ptr)->e_shnum;
  bin->shentsize = ((t_Elf64_Hdr *)hdr_ptr)->e_shentsize;
  bin->shstrndx = ((t_Elf64_Hdr *)hdr_ptr)->e_shstrndx;

  munmap(hdr_ptr, sizeof(t_Elf64_Hdr));
  if (version != bin->b_version)
  {
    log_error(ERR_NO_FORMAT, bin->b_path);
    return;
  }
  PRINT_HEADER((t_Elf64_Hdr *)hdr_ptr);
  parser_elf_section(bin, ctx);
}
