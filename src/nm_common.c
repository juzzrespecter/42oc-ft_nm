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

/**
 * Generamos un nuevo nodo cuyo contenido es una cobertura que incluye un puntero
 * a la memoria copiada del símbolo, el índice de la cabecera que apunta a la
 * strtab que contiene el nombre del símbolo.
 * @param sym ;; símbolo extraído del binario
 * @param sh_link ;; índice de la tabla de cadenas de la sección de símbolos.
 * @param ctx ;; contexto del programa.
 * @return
 */
t_list *build_new_sym_node(void *sym, ei_class b_class, uint32_t sh_link, t_nm *ctx)
{
    size_t             sym_size = (b_class == ELFCLASS64) ? sizeof(t_Elf64_Sym) : sizeof(t_Elf32_Sym);
    void              *sym_content = ft_calloc(1, sym_size);
    t_Elf_Sym_wrapper *sym_wrapper = ft_calloc(1, sizeof(t_Elf_Sym_wrapper));
    t_list *sym_node;

    if (sym_content == NULL || sym_wrapper == NULL)
    {
        free(sym_content);
        free(sym_wrapper);
        log_and_exit(ERR_SYS, NULL, ctx);
    }
    ft_memcpy(sym_content, sym, sym_size);
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

t_list *build_new_shdr_node(void* shdr, ei_class class, t_nm* ctx)
    {
  printf("TEST: (%ld,%ld)\n", sizeof(t_Elf32_Shdr), sizeof(t_Elf64_Shdr));
  size_t  shdr_size = (class == ELFCLASS64) ? sizeof(t_Elf64_Shdr) : sizeof(t_Elf32_Shdr);
  void   *shdr_content = malloc(shdr_size);
  t_list *shdr_node;

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

  static int rev_symbol_cmp(unsigned long a, unsigned long b)
{
    return b - a;
}

static int symbol_cmp(unsigned long a, unsigned long b)
{
  return a - b;
}

/**
* Usamos un bubble sort de mierda para ordenar la lista en base a criterio por argumento.
*/
static void sort_symbols_alpha(t_list** alst, int (*cmp)(unsigned long, unsigned long))
{
  size_t len = ft_lstsize(*alst);
  t_list *head = *alst;
  t_list *node, *prev, *next;

  for (size_t i = 0; i < len; i++)
    {
    node = head;
    prev = head;
    while (node != NULL)
    {
      next = node->next;
      if (next == NULL)
        break ;
      t_symbol *sym = node->content;
      t_symbol *sym_next = next->content;
      if (cmp((unsigned long)sym->sym_ptr, (unsigned long)sym_next->sym_ptr) < 0)
      {
        if (node == head)
        {
            *alst = next;
            node->next = next->next;
            next->next = node;
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

// aqui deberiamos trabajar los simbolos

// -a: printa todos los simbolos (cuales no deberiamos printar ??
// se debe ordenar la lista de simbolos antes
// -g: solo simbolos externos (globales, weakos)
// -u: solo undefined (undefined subgrupo de externos ??)
// -p: no lanzamos la ordenacion
// -r: reversa sorta

void output_nm_symbols(t_bin* bin, t_nm* ctx)
{
    t_list *node = bin->b_nm_sym_lst;

    if (!(ctx->flags & NO_SORT_F) && ctx->flags & REV_SORT_F)
      sort_symbols_alpha(&bin->b_nm_sym_lst, rev_symbol_cmp);
    if (!(ctx->flags & NO_SORT_F) && !(ctx->flags & REV_SORT_F))
      sort_symbols_alpha(&bin->b_nm_sym_lst, symbol_cmp);

    write(STDOUT_FILENO, bin->b_src, ft_strlen(bin->b_src)); // esto solo printa cuando hay mas de uno
    write(STDOUT_FILENO, "\n", 1);
    for (size_t i = 0; node != NULL; i++, node=node->next) // probar esto, la i pa que
    {
        t_symbol *nm_sym = (t_symbol *)node->content;

        if (nm_sym->sym_type == 'U')
        {
            if (bin->b_class == ELFCLASS32)
              write(STDOUT_FILENO, "        ", 8);
            if (bin->b_class == ELFCLASS64)
              write(STDOUT_FILENO, "                ", 16);
        }
        else {
            if (bin->b_class == ELFCLASS32)
              printf("%08lx", (unsigned long)nm_sym->sym_ptr); // to write
            if (bin->b_class == ELFCLASS64)
              printf("%016ld", (unsigned long)nm_sym->sym_ptr); // to write
        }
        printf(" %c %s\n",nm_sym->sym_type, nm_sym->sym_name); //to write
        node = node->next;
    }
}