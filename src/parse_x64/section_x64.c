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

t_list *build_new_sym_node(t_Elf64_Sym *sym, t_Elf64_Shdr *shdr, t_nm *ctx)
{
  void              *sym_content = ft_calloc(1, sizeof(t_Elf64_Sym));
  t_elf_sym_wrapper *node_wrapper = ft_calloc(1, sizeof(t_elf_sym_wrapper));
  t_list            *sym_node;

  if (sym_content == NULL || node_wrapper == NULL)
  {
    free(sym_content);
    free(node_wrapper);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  ft_memcpy(sym_content, sym, sizeof(t_Elf64_Sym));
  node_wrapper->sym_ptr = sym_content;
  node_wrapper->shdr_ptr = shdr;
  sym_node = ft_lstnew(node_wrapper);
  if (sym_node == NULL)
  {
    free(sym_content);
    free(node_wrapper);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  return sym_node;
}

static void TEST_print_symbols(t_bin *b)
{
  t_list *n = b->b_sym_lst;

  for (size_t i = 0; n != NULL; i++)
  {
    print_symbol_table_x64((t_elf_sym_wrapper *)n->content, b, i + 1);
    n = n->next;
  }
}

static void TEST_print_sections(t_bin *b)
{
  t_list *n = b->b_elf_shdr;

  for (int i = 0; n != NULL; i++)
  {
    print_section_values_x64((t_Elf64_Shdr *)n->content, b, i + 1);
    n = n->next;
  }
}

/**
 * Seleccionamos el strtab de entre la lista de tablas obtenidas de la lista de cabeceras.
 * Si no se encuentra la tabla seleccionada, hay un fallback a .shstrtab.
 * Si no se encuentra ninguna tabla, devolvemos nulo.
 * @param index ;; identificador de la tabla
 * @param bin   ;; binario
 * @return      ;; tabla seleccionada || NULL
 */
char *select_strtab(size_t index, t_bin *bin)
{
  char   *selected_strtab = NULL;
  char   *shstrtab = NULL;
  t_list *node = bin->b_strtab_lst;

  while (node != NULL)
  {
    t_strtab *n_strtab = node->content;
    if (n_strtab->strtab_index == index)
      selected_strtab = n_strtab->strtab;
    if (n_strtab->strtab_index == bin->shstrndx)
      shstrtab = n_strtab->strtab;
    node = node->next;
  }
  if (selected_strtab == NULL)
    selected_strtab = shstrtab;
  return selected_strtab;
} // esto debe ir a common

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

// importante definir logica de string tables; de momento tenemos tres distintas,
// la de simbolos, la de secciones (referenciada en la cabecera ELF), y la de simbolos dimanicos
// Compilando codigo c++ se reducen al menos las dos primeras a una sola, investigar esto

// .shstrtab, .strtab, dyntab
// .sh_link -> en cabecera de simbolos, apunta a la strtab que contiene los nombres
// por tanto, debemos guardar este valor, o al menos las cabeceras de simbolos
// (la lista de simbolos deberian de accederse desde las cabeceras de simbolos ?? )
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
  for (size_t i = 0; i < n; i++)
  {
    sym_offset = (t_Elf64_Sym *)((char *)sym + (sizeof(t_Elf64_Sym) + (i * shdr->sh_entsize)));
    printf("OFFSET: %ld, init: %ld\n", sym_offset - sym, shdr->sh_offset);
    sym_node = build_new_sym_node(sym_offset, shdr, ctx);
    ft_lstadd_back(&bin->b_sym_lst, sym_node);
  }
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
  while (i < bin->shnum)
  {
    t_Elf64_Shdr *shdr = (t_Elf64_Shdr *)((char *)shdr_ptr + bin->shentsize * i);

    shdr_node = build_new_shdr_node(shdr, ctx);
    ft_lstadd_back(&bin->b_elf_shdr, shdr_node);

    if (shdr->sh_type == SHT_STRTAB)
      parse_strtab(shdr, i, bin, ctx);
    if (shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM)
       parse_symbol_table_x64(shdr, bin, ctx);
    i++;
  }
  TEST_print_sections(bin);
  TEST_print_symbols(bin);
}

// como implementar la refwerencia a tabla desde simbolo ??
// * cada cabecera de simbolos contiene la propia lista, la obtencion de informacion se hace de cabecera en cabereca
// * referencia superficial dentro de cada simbolo, desacopla la lista de simbolos de la de cabeceras -> THIS
// * sin lista de cabeceras, guardamos info en el nodo del simbolo
