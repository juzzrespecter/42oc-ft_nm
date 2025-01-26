# include "../../include/nm.h"
# include "../../include/nm_x64.h"

static const char *special_sections[6] = {".data", ".data1", ".bss", ".text", ".sdata", ".sbss"};
// mover a common

static t_symbol *build_new_nm_symbol(t_Elf_Sym_wrapper *wr_sym, t_bin *bin, t_nm *ctx)
{
    char *shstrtab = select_strtab(bin->shstrndx, bin);
    char *strtab = select_strtab(wr_sym->sh_link, bin);
    char *sh_name;

    t_Elf64_Sym  *sym = wr_sym->sym;
    t_Elf64_Shdr *shdr = find_section_header_x64(sym->st_shndx, bin);
    t_symbol     *ft_nm_sym = ft_calloc(1, sizeof(t_symbol));

    if (ft_nm_sym == NULL)
        log_and_exit(ERR_SYS, NULL, ctx);
    if (shstrtab == NULL || strtab == NULL)
    {
        free(ft_nm_sym);
        log_and_exit(ERR_NO_FORMAT, bin->b_path, ctx);
    }
    st_bind bind_value = ELF64_ST_BIND(sym->st_info);
    st_type type_value = ELF64_ST_TYPE(sym->st_info);

    ft_nm_sym->sym_ptr = (void *)sym->st_value;
    if (type_value == STT_SECTION) // control de existencia de shstrtab ??
      ft_nm_sym->sym_name = &shstrtab[shdr->sh_name];
    else
      ft_nm_sym->sym_name = &strtab[sym->st_name];

    if (sym->st_shndx > SHN_UNDEF && sym->st_shndx < SHN_LORESERVE && shstrtab)
    {
        sh_name = &shstrtab[shdr->sh_name];

        for (int i = 0; i < 5; i++)
        {
            if (!ft_strncmp(sh_name, special_sections[i], ft_strlen(special_sections[i])))
            {
                char chosen_type = "ddbtgs"[i];

                ft_nm_sym->sym_type = chosen_type - 32 * (STB_LOCAL != bind_value);
                goto save_symbol;
            }
        }
    }

    // A: valor de simbolo absoluto, no sera cambiado en linkeo
    if (sym->st_shndx == SHN_ABS) // no me fio de esto
        ft_nm_sym->sym_type = 'a' - 32 * (STB_LOCAL != bind_value);

    //  C/c: el simbolo es comun
    if (sym->st_shndx == SHN_COMMON)
        ft_nm_sym->sym_type = 'c' - 32 * (STB_LOCAL != bind_value);

    if (type_value == STT_OBJECT)
      ft_nm_sym->sym_type = 'd' - 32 * (STB_LOCAL != bind_value);
    // simbolo es una funcion indirecta: DOCUMENTAR FUNCION INDIRECTA, (i)
    // si ha sido referenciada por una relocacion, no evalua su direccion de memeoria, sino que se
    // invoca en runtime (--ifunc-chars)

    // i (chiquita)

    // I: simbolo es funcion indirecta de otro

    // N: simbolo es un debugo simbolo: DOCUMENTAR DEBUGO SIMBOLO

    // p: simbolo en seccion de stack unwind

    // R/r: simbolo en una seccion de solo lectura (etendemos cualquier seccion read only excepto rodata{se ve que no})
    if (shdr && !(shdr->sh_flags & SHF_WRITE))
       ft_nm_sym->sym_type = 'r' - 32 * (STB_LOCAL != bind_value);

    // U: simboo sin definir (externo)
    if (sym->st_shndx == SHN_UNDEF)
      ft_nm_sym->sym_type = 'U';

    // u: Unique global symbol.

    // V/v: simbolo es un objeto guaco.

    // ...
    if (bind_value == STB_WEAK)
      ft_nm_sym->sym_type = 'w' - 32 * (STB_LOCAL != bind_value);

    // necesito la diferencia entre STT_NOTYPE (no tiene valor) y los simbolos con valor 0
save_symbol:
    return ft_nm_sym;
}

void parse_symbols_to_nm_fmt_x64(t_bin *bin, t_nm *ctx)
   {
   t_list   *node = bin->b_sym_lst;
   t_symbol *nm_sym;
   t_list   *nm_sym_node;

   for (size_t i = 0; node != NULL; i++)
   {
       nm_sym = build_new_nm_symbol((t_Elf_Sym_wrapper *)node->content, bin, ctx);
       nm_sym_node = ft_lstnew(nm_sym);
       ft_lstadd_back(&bin->b_nm_sym_lst, nm_sym_node);
       node = node->next;
   }
    output_nm_symbols(bin, ctx);
  }