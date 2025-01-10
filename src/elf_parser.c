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

/**
 * Parses ELF header identity, saves encoding, class and version into
 * the binary context structure, no matter what format it is.
 *
 * @param bin Current binary context
 * @param ctx Global context
 * @return true if success, false if could not read identity bytes from binary
 */
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

/**
*  Parses ei_ident section of the ELF header, then selects the actual architecture
*  of the binary and calls the correct class function.
*
* @param bin: current binary context.
* @param ctx: program context.
*/
void parser_elf(t_bin* bin, t_nm* ctx)
{
  if (!__parser_elf_header_ident(bin, ctx))
    return ;
  if (bin->b_class == ELFCLASS32)
    parser_elf_hdr_x32(bin, ctx);
  else if (bin->b_class == ELFCLASS64)
    parser_elf_hdr_x64(bin, ctx);
}
