#include "../../include/nm_x64.h"
#include "../../include/nm.h"

void parser_elf_hdr_x64(t_bin* bin, t_nm* ctx)
{
  t_Elf64_Hdr* hdr_ptr = bin->b_src;
  uint32_t version;

  version = ((t_Elf64_Hdr*)hdr_ptr)->e_version;
  if (version != bin->b_version)
  {
    log_error(ERR_NO_FORMAT, bin->b_path);
    return;
  }

  bin->shoff = ((t_Elf64_Hdr*)hdr_ptr)->e_shoff;
  bin->shnum = ((t_Elf64_Hdr*)hdr_ptr)->e_shnum;
  bin->shentsize = ((t_Elf64_Hdr*)hdr_ptr)->e_shentsize;
  bin->shstrndx = ((t_Elf64_Hdr*)hdr_ptr)->e_shstrndx;

  parser_elf_section_x64(bin, ctx);
  parse_symbols_to_nm_fmt_x64(bin, ctx);
}
