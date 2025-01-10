#include "../../include/nm.h"
#include "../../include/nm_x64.h"

/*
static char *special_sections[22] = {
  ".bss", ".comment", ".data", ".data1",
  ".debug", ".dynamic", ".dynstr", ".dynsym",
  ".fini", ".got", ".hash", ".init",
  ".interp", ".line", ".node", ".plt",
  ".rodata", ".rodata1", ".shstrtab", ".strtab",
  ".symtab", ".text"
  };*/

t_list *build_new_sym_node(t_Elf64_Sym *sym, t_nm *ctx)
{
  void   *sym_content = malloc(sizeof(t_Elf64_Sym));
  t_list *sym_node;

  if (sym_content == NULL)
    log_and_exit(ERR_SYS, NULL, ctx);
  ft_memcpy(sym_content, sym, sizeof(t_Elf64_Sym));
  sym_node = ft_lstnew(sym_content);
  if (sym_node == NULL)
  {
    free(sym_content);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  return sym_node;
}

void TEST_print_strtab(t_Elf64_Shdr *shdr, int fd)
{
  void *section;
  off_t pa_offset = shdr->sh_offset & ~(sysconf(_SC_PAGESIZE) - 1);
  off_t tru_offset = shdr->sh_offset - pa_offset;

  printf("Lo de los offsets: (off: %ld, pa: %ld, tru off: %ld)\n", shdr->sh_offset, pa_offset, tru_offset);
  section = mmap(NULL, shdr->sh_size + tru_offset, PROT_WRITE, MAP_PRIVATE, fd, pa_offset);
  if (section == MAP_FAILED)
  {
    perror("TEST_print_strtab");
    exit(1);
  }
  section = section + tru_offset + 1;
  int i = 0;
  while ((section + i) < section + shdr->sh_size)
  {
    write(1, section + i, 1);
    i++;
  }
}

static void parse_string_table(t_Elf64_Shdr* shdr, t_bin* bin, t_nm* ctx)
{
// tamano a leer
  //t_Elf64_Xword len = shdr->sh_size;
  (void)shdr, (void)bin, (void)ctx;

  //bin->b_strtab


}

static void parse_symbol_table(t_Elf64_Shdr *shdr, t_bin* bin, t_nm *ctx)
{
  t_Elf64_Sym *sym;
  t_Elf64_Sym *sym_offset;
  t_list      *sym_node;

  off_t pa_offset = shdr->sh_offset & ~(sysconf(_SC_PAGESIZE) - 1);
  off_t tru_offset = shdr->sh_offset - pa_offset;

  sym = mmap(NULL, shdr->sh_size + tru_offset, PROT_READ, MAP_PRIVATE, bin->b_fd, pa_offset);
  printf("OFFSET: %ld, PA_OFFSET: %ld, TRUE: %ld\n", shdr->sh_offset, pa_offset, tru_offset);
  if (sym == MAP_FAILED)
    log_and_exit(ERR_SYS, NULL, ctx); // leak en mmap superior ??
  //sym += tru_offset;

  size_t i = 0;
  size_t n = shdr->sh_size / shdr->sh_entsize;

  // test to remove
  int ret = munmap(sym, shdr->sh_size + tru_offset);
  if (ret < 0)
  {
    perror("parse_symbol_table");
    exit(1);
  }
  struct stat st;
  fstat(bin->b_fd, &st);
  sym = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, bin->b_fd, 0);
  if (sym == MAP_FAILED)
  {
    perror("parse_symbol_table");
    exit(1);
  }
  printf("size: %ld\n", st.st_size);
  while (i < n)
  {
    sym_offset = (t_Elf64_Sym *)((char *)sym + shdr->sh_offset + (sizeof(t_Elf64_Sym) * i));
    printf("OFFSET: %ld, init: %ld\n", sym_offset - sym, shdr->sh_offset);
    sym_node = build_new_sym_node(sym_offset, ctx);
    ft_lstadd_back(&bin->b_sym_lst, sym_node);

    print_symbol_table_x64(sym_offset, i);
    i++;
  }
}

static t_list *build_new_shdr_node(t_Elf64_Shdr *shdr, t_nm *ctx)
{
  void   *shdr_content = malloc(sizeof(t_Elf64_Shdr));
  t_list *shdr_node;

  if (shdr_content == NULL)
    log_and_exit(ERR_SYS, NULL, ctx);
  ft_memcpy(shdr_content, shdr, sizeof(t_Elf64_Shdr));
  shdr_node = ft_lstnew(shdr_content);
  if (shdr_node == NULL)
  {
    free(shdr_content);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  return shdr_node;
}

/* Las secciones que contienen listas de símbolos son SHT_SYMTAB y SHT_DYNSYM */
void parser_elf_section_x64(t_bin *bin, t_nm *ctx) {
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
  while (i < bin->shnum) // proteccion de acceso a memoria indebido (nos estamos fiando de que shnum este bien formateado)
  {
    t_Elf64_Shdr *shdr = shdr_ptr + bin->shentsize * i;

    shdr_node = build_new_shdr_node(shdr, ctx);
    ft_lstadd_back(&bin->b_elf_shdr, shdr_node);

    if (shdr->sh_type == SHT_STRTAB)
    {
      parse_string_table(shdr, bin, ctx);
    }

    if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM)
    {
      print_section_values_x64(shdr, i);
       parse_symbol_table(shdr, bin, ctx);
    }
    i++;
  }
}