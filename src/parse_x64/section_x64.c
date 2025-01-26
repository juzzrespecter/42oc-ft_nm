#include "../../include/nm.h"
#include "../../include/nm_x64.h"

static void TEST_print_symbols(t_bin *b)
{
  t_list *n = b->b_sym_lst;

  for (size_t i = 0; n != NULL; i++)
  {
    print_symbol_table_x64((t_Elf_Sym_wrapper *)n->content, b, i);
    n = n->next;
  }
}

static void TEST_print_sections(t_bin *b)
{
  t_list *n = b->b_elf_shdr;

  for (int i = 0; n != NULL; i++)
  {
    print_section_values_x64((t_Elf64_Shdr *)n->content, b, i);
    n = n->next;
  }
}

t_Elf64_Shdr *find_section_header_x64(size_t shndx, t_bin *bin)
{
  t_list *node = bin->b_elf_shdr;
  t_Elf64_Shdr *selected_shdr = NULL;

  if (shndx == SHN_UNDEF || shndx >= SHN_LORESERVE)
    return NULL;
  for (size_t i = 0; node != NULL; i++)
  {
    if (i == shndx)
    {
      selected_shdr = (t_Elf64_Shdr *)node->content;
      break ;
    }
    node = node->next;
  }
  return selected_shdr;
}

static void parse_strtab(t_Elf64_Shdr *shdr, size_t strtab_index, t_bin* bin, t_nm *ctx)
{
  void     *shdr_strtab = bin->b_src + shdr->sh_offset;
  t_strtab *strtab = ft_calloc(1, sizeof(t_strtab));
  t_list   *strtab_node = ft_calloc(1, sizeof(t_list));;

  if (!strtab || !strtab_node)
    log_and_exit(ERR_SYS, NULL, ctx);
  strtab->strtab = malloc(shdr->sh_size * sizeof(char));
  if (!strtab->strtab)
  {
    free(strtab);
    free(strtab_node);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  ft_memcpy(strtab->strtab, shdr_strtab, shdr->sh_size);
  strtab->strtab_index = strtab_index;
  strtab_node->content = (void *)strtab;

  ft_lstadd_back(&bin->b_strtab_lst, strtab_node);
}

static void parse_symbol_table_x64(t_Elf64_Shdr *shdr, t_bin* bin, t_nm *ctx)
{
  t_Elf64_Sym *sym;
  t_Elf64_Sym *sym_offset;
  t_list      *sym_node;
  size_t       n = shdr->sh_size / shdr->sh_entsize;

  sym = (t_Elf64_Sym *)((char *)bin->b_src + shdr->sh_offset);
  for (size_t i = 1; i < n; i++)
  {
    sym_offset = (t_Elf64_Sym *)((char *)sym + shdr->sh_entsize * i);
    sym_node = build_new_sym_node(sym_offset, bin->b_class, shdr->sh_link, ctx);
    ft_lstadd_back(&bin->b_sym_lst, sym_node);
  }
}

static void parser_elf_section_x64(t_bin *bin, t_nm *ctx) {
  void   *shdr_ptr;
  t_list *shdr_node;
  ssize_t shdr_len = bin->shnum * bin->shentsize;
  size_t  i = 0;

  if (bin->shoff + shdr_len > bin->b_size)
  {
     log_error(ERR_NO_FORMAT, bin->b_path);
     return;
  }
  shdr_ptr = bin->b_src + bin->shoff;
  while (i < bin->shnum)
  {
    t_Elf64_Shdr *shdr = (t_Elf64_Shdr *)((char *)shdr_ptr + bin->shentsize * i);

    shdr_node = build_new_shdr_node(shdr, bin->b_class, ctx);
    ft_lstadd_back(&bin->b_elf_shdr, shdr_node);

    if (shdr->sh_type == SHT_STRTAB)
      parse_strtab(shdr, i, bin, ctx);
    if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM)
       parse_symbol_table_x64(shdr, bin, ctx);
    i++;
  }
# ifdef DEBUG
  TEST_print_sections(bin);
  TEST_print_symbols(bin);
# endif
}

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
