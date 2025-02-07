# include "../include/nm.h"
# include "../include/nm_x32.h"
# include "../include/nm_x64.h"

/**
 * Seleccionamos el strtab de entre la lista de tablas obtenidas de la lista de cabeceras.
 * Si no se encuentra la tabla seleccionada, hay un fallback a .shstrtab.
 * Si no se encuentra ninguna tabla, devolvemos nulo.
 * @param index ;; identificador de la tabla
 * @param bin   ;; binario
 * @return      ;; tabla seleccionada || NULL
 */
char* select_strtab(size_t index, t_bin* bin)
{
  char* selected_strtab = NULL;
  char* shstrtab = NULL;
  t_list* node = bin->b_strtab_lst;

  while (node != NULL)
  {
    t_strtab* n_strtab = node->content;
    if (n_strtab->strtab_index == index)
      selected_strtab = n_strtab->strtab;
    if (n_strtab->strtab_index == bin->shstrndx)
      shstrtab = n_strtab->strtab;
    node = node->next;
  }
  if (selected_strtab == NULL)
    selected_strtab = shstrtab;
  return selected_strtab;
}

/**
 * Generamos un nuevo nodo cuyo contenido es una cobertura que incluye un puntero
 * a la memoria copiada del símbolo, el índice de la cabecera que apunta a la
 * strtab que contiene el nombre del símbolo.
 * @param sym ;; símbolo extraído del binario
 * @param sh_link ;; índice de la tabla de cadenas de la sección de símbolos.
 * @param ctx ;; contexto del programa.
 * @return
 */
t_list* build_new_sym_node(void* sym, ei_class b_class, uint32_t sh_link, t_nm* ctx)
{
  size_t sym_size = (b_class == ELFCLASS64) ? sizeof(t_Elf64_Sym) : sizeof(t_Elf32_Sym);
  void* sym_content = ft_calloc(1, sym_size);
  t_Elf_Sym_wrapper* sym_wrapper = ft_calloc(1, sizeof(t_Elf_Sym_wrapper));
  t_list* sym_node;

  if (sym_content == NULL || sym_wrapper == NULL)
  {
    free(sym_content);
    free(sym_wrapper);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  ft_memcpy(sym_content, sym, sym_size);
  sym_wrapper->sh_link = sh_link;
  sym_wrapper->sym = sym_content;
  sym_node = ft_lstnew(sym_wrapper);
  if (sym_node == NULL)
  {
    free(sym_content);
    free(sym_wrapper);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  return sym_node;
}

t_list* build_new_shdr_node(void* shdr, ei_class class, t_nm* ctx)
{
  size_t shdr_size = (class == ELFCLASS64) ? sizeof(t_Elf64_Shdr) : sizeof(t_Elf32_Shdr); // macro get size
  void* shdr_content = malloc(shdr_size);
  t_list* shdr_node;

  if (shdr_content == NULL)
    log_and_exit(ERR_SYS, NULL, ctx);
  ft_memcpy(shdr_content, shdr, shdr_size);
  shdr_node = ft_lstnew(shdr_content);
  if (shdr_node == NULL)
  {
    free(shdr_content);
    log_and_exit(ERR_SYS, NULL, ctx);
  }
  return shdr_node;
}

static int ft_strcnmp_rev(const char* a, const char* b, size_t len)
{
  return -1 * ft_strncmp(a, b, len);
}

/**
* Usamos un bubble sort de mierda para ordenar la lista en base a criterio por argumento.
*/
static void sort_symbols_alpha(t_list** alst, int (*cmp)(const char*, const char*, size_t))
{
  size_t len = ft_lstsize(*alst);
  t_list *node, *prev, *next;

  for (size_t i = 0; i < len; i++)
  {
    node = *alst;
    prev = *alst;
    while (node != NULL)
    {
      next = node->next;
      if (next == NULL)
        break ;
      t_symbol* sym = node->content;
      t_symbol* sym_next = next->content;
      if (cmp(sym->sym_name, sym_next->sym_name, ft_strlen(sym->sym_name) + 1) < 0)
      {
        if (node == *alst)
        {
          *alst = next;
          node->next = (*alst)->next;
          (*alst)->next = node;
        }
        else
        {
          prev->next = next;
          node->next = next->next;
          next->next = node;
        }
      }
      prev = node;
      next = next->next;
      node = node->next;
    }
  }
}

void print_value(char *sym_buffer, unsigned long value, int class, st_shndx shn)
{
  int  end = (class == ELFCLASS32) ? 8 : 16;
  char pad = (shn == SHN_UNDEF) ? ' ' : '0';

  ft_memset(sym_buffer, pad, 16);
  for (int i = 15; i >= 0 && value && shn != SHN_UNDEF; value = value / 16)
    sym_buffer[i--] = "0123456789abcdef"[value % 16];
  if (class == ELFCLASS32 && shn != SHN_UNDEF)
    ft_memmove(sym_buffer, sym_buffer + 8, 8);
  sym_buffer[end] = 0;
}

void output_nm_symbols(t_bin* bin, t_nm* ctx)
{
  t_list*   node;
  t_symbol* nm_sym;
  char      sym_buffer[17];

  if (!(ctx->flags & NO_SORT_F) && ctx->flags & REV_SORT_F)
    sort_symbols_alpha(&bin->b_nm_sym_lst, ft_strncmp);
  if (!(ctx->flags & NO_SORT_F) && !(ctx->flags & REV_SORT_F))
    sort_symbols_alpha(&bin->b_nm_sym_lst, ft_strcnmp_rev);
  node = bin->b_nm_sym_lst;

  if (ctx->b_lst->next)
    printf("\n%s:\n", bin->b_path);
  for (; node != NULL; node = node->next)
  {
    nm_sym = (t_symbol*)node->content;
    print_value(sym_buffer,nm_sym->sym_value, bin->b_class, nm_sym->shndx);
    printf("%s %c %s\n", sym_buffer, nm_sym->sym_type, nm_sym->sym_name);
  }
}
