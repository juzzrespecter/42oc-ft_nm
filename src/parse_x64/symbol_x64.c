# include "../../include/nm.h"
# include "../../include/nm_x64.h"

static t_symbol *build_new_nm_symbol(t_Elf_Sym_wrapper *wr_sym, t_bin *bin, t_nm *ctx)
{

    char *shstrtab = select_strtab(bin->shstrndx, bin);
    char *strtab = select_strtab(wr_sym->sh_link, bin);

    t_Elf64_Sym  *sym = wr_sym->sym;
    t_Elf64_Shdr *shdr = find_section_header_x64(sym->st_shndx, bin);
    t_symbol     *ft_nm_sym = ft_calloc(1, sizeof(t_symbol));

    if (ft_nm_sym == NULL)
        log_and_exit(ERR_SYS, NULL, ctx);
    if (shstrtab == NULL || strtab == NULL)
    {
        free(ft_nm_sym);;
        log_and_exit(ERR_NO_FORMAT, bin->b_path, ctx);
    }
    st_bind bind_value = ELF64_ST_BIND(sym->st_info);
    st_type type_value = ELF64_ST_TYPE(sym->st_info);
    (void) type_value; // TODO remove

    ft_nm_sym->sym_ptr = (void *)sym->st_value;
    ft_nm_sym->sym_name = &strtab[sym->st_name];

    // A: valor de simbolo absoluto, no sera cambiado en linkeo
    if (shdr->sh_name == SHN_ABS) // no me fio de esto
    {

    }
    // B/b: simbolo en .bss data section
    if (!ft_strncmp(&shstrtab[shdr->sh_name], ".bss", 4))
    {
        // como comprobamos que el simbolo esta en .bss ?? (.bss no es una cabecera de simbolos)
    }
    //  C/c: el simbolo es comun

    // D/d: el simbolo esta en la seccion iniializada de data


    if (!ft_strncmp(&shstrtab[shdr->sh_name], ".data", 6) && (bind_value == STB_GLOBAL || bind_value == STB_LOCAL))
    {
        ft_nm_sym->sym_type = (bind_value == STB_GLOBAL) ? 'D' : 'd';
    }

    // G/g: en la seccion inicializada de data en seccion esmalla objecto

    // simbolo es una funcion indirecta: DOCUMENTAR FUNCION INDIRECTA, (i)
    // si ha sido referenciada por una relocacion, no evalua su direccion de memeoria, sino que se
    // invoca en runtime (--ifunc-chars)

    // I: simbolo es funcion indirecta de otro

    // N: simbolo es un debugo simbolo: DOCUMENTAR DEBUGO SIMBOLO

    // ...

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
  }