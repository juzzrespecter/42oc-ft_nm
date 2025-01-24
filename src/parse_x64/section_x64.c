#include "../../include/nm.h"
#include "../../include/nm_x64.h"

/**
 * Generamos un nuevo nodo cuyo contenido es una cobertura que incluye un puntero
 * a la memoria copiada del símbolo, el índice de la cabecera que apunta a la
 * strtab que contiene el nombre del símbolo.
 * @param sym ;; símbolo extraído del binario
 * @param sh_link ;; índice de la tabla de cadenas de la sección de símbolos.
 * @param ctx ;; contexto del programa.
 * @return
 */
static t_list *build_new_sym_node(t_Elf64_Sym *sym, t_Elf64_Word sh_link, t_nm *ctx)
{
  void              *sym_content = ft_calloc(1, sizeof(t_Elf64_Sym));
  t_Elf_Sym_wrapper *sym_wrapper = ft_calloc(1, sizeof(t_Elf_Sym_wrapper));
  t_list *sym_node;

  if (sym_content == NULL || sym_wrapper == NULL)
  {
    free(sym_content);
    free(sym_wrapper);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  ft_memcpy(sym_content, sym, sizeof(t_Elf64_Sym));
  sym_wrapper->sh_link = sh_link;
  sym_wrapper->sym = sym;
  sym_node = ft_lstnew(sym_wrapper);
  if (sym_node == NULL)
  {
    free(sym_content);
    free(sym_wrapper);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  return sym_node;
}

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
  printf("Gardando strtabs: indice -> %ld\n", strtab_index);
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
    sym_offset = (t_Elf64_Sym *)((char *)sym + shdr->sh_entsize * i);
    sym_node = build_new_sym_node(sym_offset, shdr->sh_link, ctx);
    ft_lstadd_back(&bin->b_sym_lst, sym_node);
  }
}

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
  TEST_print_sections(bin); // proteger para debug
  TEST_print_symbols(bin); // proteger para debug
}
